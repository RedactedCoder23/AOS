# AOS — tests/python/test_metrics_crossplatform.py — Purpose: Python module 
# AOS — ./tests/python/test_metrics_crossplatform.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from scripts import agent_orchestrator


class CrossPlatformMetricsTest(unittest.TestCase):
    def test_with_psutil(self):
        self.assertGreaterEqual(agent_orchestrator._cpu_pct(), 0.0)
        self.assertGreaterEqual(agent_orchestrator._mem_pct(), 0.0)

    def test_without_psutil(self):
        ps = agent_orchestrator.psutil
        agent_orchestrator.psutil = None
        self.assertEqual(agent_orchestrator._cpu_pct(), 0.0)
        self.assertEqual(agent_orchestrator._mem_pct(), 0.0)
        agent_orchestrator.psutil = ps


if __name__ == "__main__":
    unittest.main()
