/* AOS — tests/test_benchmark.py — Purpose: Python module */
# AOS — ./tests/test_benchmark.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from fastapi.testclient import TestClient
from src.api.metrics import app as metrics_app
from src.branch.merge import three_way_merge
from src import fs


def test_metrics_benchmark(benchmark):
    client = TestClient(metrics_app)
    benchmark(lambda: client.get("/branches/0/metrics", headers={"Role": "admin"}))


def test_merge_benchmark(benchmark):
    benchmark(lambda: three_way_merge("a", "b", "c"))


def test_fs_benchmark(tmp_path, benchmark):
    path = tmp_path / "file"
    benchmark(lambda: fs.write_file(path.as_posix(), b"x"))
