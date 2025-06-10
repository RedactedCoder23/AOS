import unittest
from unittest import mock

from scripts import ai_cred_client


class AiCredClientTest(unittest.TestCase):
    def _mock_socket(self, response_bytes):
        sock = mock.MagicMock()
        sock.__enter__.return_value = sock
        sock.recv.return_value = response_bytes
        return sock

    def test_get_api_key_success(self):
        sock = self._mock_socket(b'{"result": "abc"}')
        with mock.patch("socket.socket", return_value=sock):
            key = ai_cred_client.get_api_key("openai")
        self.assertEqual(key, "abc")
        sock.connect.assert_called_once_with(ai_cred_client.SOCK_PATH)
        sock.sendall.assert_called()

    def test_get_api_key_missing(self):
        sock = self._mock_socket(b'{"result": ""}')
        with mock.patch("socket.socket", return_value=sock):
            with self.assertRaises(KeyError):
                ai_cred_client.get_api_key("openai")


if __name__ == "__main__":
    unittest.main()
