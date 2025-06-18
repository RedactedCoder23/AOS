# AOS — scripts/tests/test_agent_orchestrator_advanced.py — Purpose: Python module 
# AOS — ./scripts/tests/test_agent_orchestrator_advanced.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import json
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_orchestrator, branch_ui  # noqa: E402


class AdvancedOrchestratorTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def _write_tasks(self, text: str):
        with open("branches/1/tasks.yaml", "w") as fh:
            fh.write(text)

    def test_auto_scaling(self):
        tasks = "\n".join(f"- agent_id: t{i}\n  command: 'echo {i}'" for i in range(6))
        self._write_tasks(tasks)
        list(agent_orchestrator.run_tasks(1))
        m = agent_orchestrator.METRICS.get(1, {})
        self.assertGreaterEqual(m.get("agents_spawned", 1), 2)

    def test_dependencies(self):
        self._write_tasks(
            "- agent_id: build-1\n  command: 'echo build'\n"
            "- agent_id: test-2\n  command: 'echo test'\n  depends_on: ['build-1']\n"
        )
        list(agent_orchestrator.run_tasks(1))
        build = json.load(
            open(os.path.join("branches", "1", "agents", "agent-build-1.json"))
        )
        test = json.load(
            open(os.path.join("branches", "1", "agents", "agent-test-2.json"))
        )
        self.assertEqual(build["status"], "success")
        self.assertEqual(test["status"], "success")

    def test_retry_logic(self):
        script = os.path.join("branches", "1", "flaky.py")
        with open(script, "w") as f:
            f.write(
                "import sys, json, os\n"
                "cnt = int(open('cnt','r').read()) if os.path.exists('cnt') else 0\n"
                "cnt += 1\nopen('cnt','w').write(str(cnt))\n"
                "sys.exit(0 if cnt>2 else 1)\n"
            )
        self._write_tasks("- agent_id: flaky\n  command: 'python flaky.py'\n")
        list(agent_orchestrator.run_tasks(1))
        res = json.load(
            open(os.path.join("branches", "1", "agents", "agent-flaky.json"))
        )
        self.assertEqual(res["status"], "success")
        self.assertGreaterEqual(
            int(open(os.path.join("branches", "1", "cnt")).read()), 3
        )

    def test_metrics_endpoint(self):
        self._write_tasks("- agent_id: ok\n  command: 'echo hi'\n")
        list(agent_orchestrator.run_tasks(1))
        branch_ui.app.config["TESTING"] = True
        client = branch_ui.app.test_client()
        res = client.get("/branches/1/metrics")
        data = json.loads(res.data)
        self.assertIn("agents_spawned", data)
        self.assertIn("success_rate", data)
        self.assertIn("avg_runtime", data)


if __name__ == "__main__":
    unittest.main()
