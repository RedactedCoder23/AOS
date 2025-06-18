# AOS — ./tests/python/test_agent_retry.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import tempfile
import unittest
import json

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_orchestrator  # noqa: E402


class AgentRetryTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def test_restart_count(self):
        script = os.path.join("branches", "1", "flaky.py")
        with open(script, "w") as f:
            f.write(
                "import sys, os\n"
                "cnt = int(open('cnt','r').read()) if os.path.exists('cnt') else 0\n"
                "cnt += 1\nopen('cnt','w').write(str(cnt))\n"
                "sys.exit(0 if cnt>1 else 1)\n"
            )
        with open("branches/1/tasks.json", "w") as fh:
            json.dump([{"agent_id": "flaky", "command": "python flaky.py"}], fh)
        list(agent_orchestrator.run_tasks(1))
        res = json.load(open("branches/1/agents/agent-flaky.json"))
        self.assertEqual(res["status"], "success")
        self.assertEqual(res["restart_count"], 1)


if __name__ == "__main__":
    unittest.main()
