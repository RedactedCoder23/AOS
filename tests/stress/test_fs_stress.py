import os
import threading
import tempfile
import time
from src import fs


def worker(path: str, stop: float) -> None:
    while time.time() < stop:
        fs.write_file(path, os.urandom(16))
        fs.read_file(path)


def test_fs_stress():
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, "file.bin")
        stop = time.time() + 2
        threads = [threading.Thread(target=worker, args=(path, stop)) for _ in range(5)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        data = fs.read_file(path)
        assert isinstance(data, bytes)
