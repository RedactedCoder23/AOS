import os
import subprocess
import unittest

SCRIPT = os.path.join("scripts", "ai_backend.py")


class AiBackendTest(unittest.TestCase):
    def test_offline(self):
        env = os.environ.copy()
        env["AOS_AI_OFFLINE"] = "1"
        res = subprocess.run(
            ["python3", SCRIPT, "hi"], capture_output=True, text=True, env=env
        )
        self.assertEqual(res.returncode, 0)
        self.assertIn("[mock-ai]", res.stdout)


if __name__ == "__main__":
    unittest.main()
