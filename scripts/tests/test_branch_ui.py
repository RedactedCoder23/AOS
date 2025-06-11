import os
import sys
import json
import unittest
import tempfile
from unittest import mock
from flask import Response

sys.path.insert(
    0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
)  # noqa: E402
from scripts import branch_ui  # noqa: E402


class BranchUITest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()
        branch_ui.service.branches.clear()
        self.audit = tempfile.NamedTemporaryFile(delete=False)
        os.environ["AOS_AUDIT_LOG"] = self.audit.name

    def tearDown(self):
        os.unlink(self.audit.name)

    @mock.patch("scripts.branch_ui.flask_sse.publish")
    @mock.patch("scripts.branch_ui.app.logger.info")
    @mock.patch("scripts.branch_ui.kernel_ipc", return_value=1)
    @mock.patch("scripts.branch_ui.subprocess.Popen")
    def test_create_and_list(self, popen, _ipc, log_info, publish):
        proc = mock.Mock()
        proc.pid = 123
        popen.return_value = proc
        res = self.client.post(
            "/branches", environ_base={"REMOTE_USER": str(os.getuid())}
        )
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
        branch_ui.service.branches[1] = {
            "pid": 1,
            "sock": "s",
            "status": "RUNNING",
            "owner_uid": os.getuid(),
        }
        res = self.client.post(
            "/branches/1/merge", environ_base={"REMOTE_USER": str(os.getuid())}
        )
        data = json.loads(res.data)
        self.assertTrue(data["merged"])
        self.assertIn("detail", data)
        publish.assert_called_with({"branch_id": 1}, event="branch-merged")

    @mock.patch("scripts.branch_ui.flask_sse.publish")
    @mock.patch("scripts.branch_ui.kernel_ipc", side_effect=[1, 42])
    @mock.patch("scripts.branch_ui.subprocess.Popen")
    def test_snapshot_flow(self, popen, ipc, publish):
        proc = mock.Mock()
        proc.pid = 1
        popen.return_value = proc
        self.client.post("/branches")
        res = self.client.post(
            "/branches/1/snapshot", environ_base={"REMOTE_USER": str(os.getuid())}
        )
        data = json.loads(res.data)
        self.assertEqual(data["snapshot_id"], 42)
        res = self.client.get("/branches")
        data = json.loads(res.data)
        self.assertEqual(data[0]["last_snapshot_id"], 42)
        publish.assert_called_with(
            {"branch_id": 1, "snapshot_id": 42}, event="branch-snapshot"
        )

    @mock.patch("scripts.branch_ui.flask_sse.publish")
    @mock.patch("scripts.branch_ui.kernel_ipc", side_effect=[1, 0])
    @mock.patch("scripts.branch_ui.subprocess.Popen")
    def test_delete_flow(self, popen, ipc, publish):
        proc = mock.Mock()
        proc.pid = 2
        popen.return_value = proc
        self.client.post("/branches")
        res = self.client.delete(
            "/branches/1", environ_base={"REMOTE_USER": str(os.getuid())}
        )
        data = json.loads(res.data)
        self.assertTrue(data["success"])
        res = self.client.get("/branches")
        data = json.loads(res.data)
        self.assertEqual(len(data), 0)
        publish.assert_called_with({"branch_id": 1}, event="branch-deleted")

    @mock.patch("scripts.branch_ui.os.getuid", return_value=9999)
    def test_permission_denied(self, fake_uid):
        branch_ui.service.branches[1] = {
            "pid": 1,
            "sock": "s",
            "status": "RUNNING",
            "owner_uid": 0,
        }
        res = self.client.post("/branches/1/snapshot")
        self.assertEqual(res.status_code, 403)

    @mock.patch("scripts.branch_ui.kernel_ipc", return_value=-22)
    def test_invalid_snapshot(self, ipc):
        res = self.client.post("/branches/99/snapshot")
        data = json.loads(res.data)
        self.assertIn("error", data)
        self.assertEqual(res.status_code, 404)

    @mock.patch("scripts.branch_ui.kernel_ipc", return_value=-22)
    def test_invalid_delete(self, ipc):
        res = self.client.delete("/branches/99")
        data = json.loads(res.data)
        self.assertIn("error", data)
        self.assertEqual(res.status_code, 404)

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
