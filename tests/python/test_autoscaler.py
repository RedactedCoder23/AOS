import os
import sys
import importlib
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts.autoscaler import daemon  # noqa: E402


class AutoscalerTest(unittest.TestCase):
    def setUp(self):
        self.orig_env = os.environ.copy()
        os.environ["AOS_RQ_MIN"] = "1"
        os.environ["AOS_RQ_MAX"] = "3"
        importlib.reload(daemon)

    def tearDown(self):
        os.environ.clear()
        os.environ.update(self.orig_env)

    def test_scale_up(self):
        q = mock.Mock()
        type(q).count = mock.PropertyMock(return_value=5)
        client = mock.Mock()
        client.containers.list.return_value = []
        daemon.check_and_scale(client, q)
        self.assertEqual(client.containers.run.call_count, 3)

    def test_scale_down(self):
        q = mock.Mock()
        type(q).count = mock.PropertyMock(return_value=0)
        c1 = mock.Mock()
        c2 = mock.Mock()
        client = mock.Mock()
        client.containers.list.return_value = [c1, c2]
        daemon.check_and_scale(client, q)
        c2.stop.assert_called_once()
        c2.remove.assert_called_once()
        self.assertFalse(c1.stop.called)

    def test_redis_down(self):
        class BadQueue:
            @property
            def count(self):
                raise Exception("down")

        q = BadQueue()
        client = mock.Mock()
        daemon.check_and_scale(client, q)
        self.assertFalse(client.containers.run.called)
        self.assertFalse(client.containers.list.called)


if __name__ == "__main__":
    unittest.main()
