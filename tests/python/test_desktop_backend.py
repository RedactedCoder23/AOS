import json
import os
import unittest

METRICS = os.path.join("examples", "metrics_sample.json")


class DesktopBackendTest(unittest.TestCase):
    def test_metrics_file(self):
        with open(METRICS) as f:
            data = json.load(f)
        self.assertIn("cpu", data)


if __name__ == "__main__":
    unittest.main()
