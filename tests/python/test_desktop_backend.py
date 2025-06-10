import json
import os
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

METRICS = os.path.join("examples", "metrics_sample.json")


class DesktopBackendTest(unittest.TestCase):
    def test_metrics_file(self):
        with open(METRICS) as f:
            data = json.load(f)
        self.assertIn("cpu", data)


if __name__ == "__main__":
    unittest.main()
