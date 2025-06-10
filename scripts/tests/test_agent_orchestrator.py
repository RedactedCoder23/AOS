import os
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts.agent_orchestrator import AgentOrchestrator


class AgentOrchestratorTest(unittest.TestCase):
    def setUp(self):
        self.orch = AgentOrchestrator(mock.Mock())

    def _script(self, content):
        fd, path = tempfile.mkstemp(suffix=".py")
        with os.fdopen(fd, "w") as f:
            f.write(content)
        os.chmod(path, 0o755)
        return path

    def test_exit_failure(self):
        script = self._script("import sys; sys.exit(1)")
        with mock.patch("scripts.agent_orchestrator._SSE.publish") as publish:
            self.orch.launch(1, 3, [sys.executable, script])
            publish.assert_called()
            args, kwargs = publish.call_args
            self.assertEqual(kwargs.get("event"), "agent-status")
            data = args[0]
            self.assertEqual(data["status"], "failed")
        os.remove(script)

    def test_timeout(self):
        script = self._script("import time; time.sleep(1)")
        with mock.patch("scripts.agent_orchestrator._SSE.publish") as publish:
            self.orch.launch(1, 4, [sys.executable, script], timeout=0.1)
            publish.assert_called()
            args, kwargs = publish.call_args
            data = args[0]
            self.assertEqual(data["status"], "failed")
            self.assertEqual(data.get("error"), "Timeout")
        os.remove(script)


if __name__ == "__main__":
    unittest.main()
