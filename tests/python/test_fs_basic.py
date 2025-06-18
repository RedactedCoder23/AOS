# AOS — tests/python/test_fs_basic.py — Purpose: Python module 
# AOS — ./tests/python/test_fs_basic.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import tempfile
import unittest
from src import fs


class FSTest(unittest.TestCase):
    def test_write_and_read(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, "file.txt")
            fs.write_file(path, b"data")
            self.assertEqual(fs.read_file(path), b"data")
            st = fs.stat(path)
            self.assertTrue(st.st_size > 0)


if __name__ == "__main__":
    unittest.main()
