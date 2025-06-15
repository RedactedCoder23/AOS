import unittest
from fastapi.testclient import TestClient
from src.api.memory import app


class MemoryApiTest(unittest.TestCase):
    def test_fragmentation_endpoint(self):
        client = TestClient(app)
        resp = client.get("/memory/fragmentation", headers={"Role": "admin"})
        self.assertEqual(resp.status_code, 200)
        self.assertIn("ratio", resp.json())


if __name__ == "__main__":
    unittest.main()
