import json
import unittest
from unittest import mock

from scripts import branch_ui
from flask import Response


class BranchUITest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()
        branch_ui.service.branches.clear()

    @mock.patch("scripts.branch_ui.flask_sse.publish")
    @mock.patch("scripts.branch_ui.app.logger.info")
    @mock.patch("scripts.branch_ui.kernel_ipc", return_value=1)
    @mock.patch("scripts.branch_ui.subprocess.Popen")
    def test_create_and_list(self, popen, _ipc, log_info, publish):
        proc = mock.Mock()
        proc.pid = 123
        popen.return_value = proc
        res = self.client.post("/branches")
        data = json.loads(res.data)
        self.assertEqual(data["branch_id"], 1)
        publish.assert_called_with({"branch_id": 1}, event="branch-created")
        log_info.assert_called_with("Launched branch %d, PID %d", 1, 123)
        res = self.client.get("/branches")
        data = json.loads(res.data)
        self.assertEqual(len(data), 1)
        self.assertEqual(data[0]["status"], "RUNNING")

    @mock.patch(
        "scripts.branch_ui.subprocess.check_output", return_value='{"patch":"diff"}'
    )
    @mock.patch("scripts.branch_ui.flask_sse.publish")
    @mock.patch("scripts.branch_ui.kernel_ipc", return_value=0)
    def test_merge(self, _ipc, publish, _chk):
        branch_ui.service.branches[1] = {"pid": 1, "sock": "s", "status": "RUNNING"}
        res = self.client.post("/branches/1/merge")
        data = json.loads(res.data)
        self.assertTrue(data["merged"])
        self.assertIn("detail", data)
        publish.assert_called_with({"branch_id": 1}, event="branch-merged")

    @mock.patch(
        "scripts.branch_ui.flask_sse.stream",
        return_value=Response("", mimetype="text/event-stream"),
    )
    def test_events(self, stream):
        res = self.client.get("/events")
        self.assertEqual(res.status_code, 200)
        self.assertEqual(res.mimetype, "text/event-stream")
        stream.assert_called_once()


if __name__ == "__main__":
    unittest.main()
