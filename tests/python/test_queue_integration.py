# AOS — tests/python/test_queue_integration.py — Purpose: Python module 
# AOS — ./tests/python/test_queue_integration.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import asyncio
import subprocess
import time
import unittest

import websockets
from fastapi.testclient import TestClient

from src.api.metrics import app as metrics_app
from src.api.coverage import app as coverage_app
from src.service import queue
from scripts import agent_orchestrator


class QueueIntegrationTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.redis_proc = subprocess.Popen(
            ["redis-server", "--port", "6379", "--daemonize", "yes"]
        )
        time.sleep(0.5)
        cls.worker_proc = subprocess.Popen(["python", "scripts/jobqueue/worker.py"])
        time.sleep(0.5)
        cls.ws_proc = subprocess.Popen(
            [
                "uvicorn",
                "src.api.ws:app",
                "--port",
                "8765",
                "--log-level",
                "critical",
            ]
        )
        time.sleep(1)
        agent_orchestrator.branch_stats["99"] = {
            "pending_tasks": 0,
            "cpu_pct": 0.0,
            "mem_pct": 0.0,
            "history": [{"timestamp": "t"}],
        }

    @classmethod
    def tearDownClass(cls):
        cls.worker_proc.terminate()
        cls.worker_proc.wait()
        cls.ws_proc.terminate()
        cls.ws_proc.wait()
        subprocess.Popen(["redis-cli", "shutdown"]).wait()

    def test_queue_lifecycle(self):
        queue.enqueue_provider_job("99", "echo", "hello")

        async def _collect():
            async with websockets.connect("ws://localhost:8765/ws/branches/99") as ws:
                msgs = []
                for _ in range(3):
                    msg = await ws.recv()
                    msgs.append(msg)
                    if "COMPLETE" in msg:
                        break
                return msgs

        msgs = asyncio.get_event_loop().run_until_complete(_collect())
        self.assertTrue(any("COMPLETE" in m for m in msgs))
        status = queue.load_status("99").get("status")
        self.assertEqual(status, "COMPLETE")
        client = TestClient(metrics_app)
        resp = client.get("/branches/99/metrics")
        if resp.status_code == 200:
            self.assertEqual(resp.json().get("status"), "COMPLETE")
        resp2 = TestClient(coverage_app).get("/branches/99/coverage-history")
        self.assertEqual(resp2.status_code, 200)
        self.assertEqual(resp2.json().get("status"), "COMPLETE")


if __name__ == "__main__":
    unittest.main()
