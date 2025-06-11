import os
import time
import json
from scripts import branch_ui


branch_ui.app.config["TESTING"] = True
client = branch_ui.app.test_client()


def main():
    runs = 100
    times = []
    for _ in range(runs):
        start = time.perf_counter()
        client.post("/branches")
        times.append(time.perf_counter() - start)
        branch_ui.service.branches.clear()
    avg = sum(times) / len(times) * 1000
    os.makedirs("branches", exist_ok=True)
    with open(os.path.join("branches", "bench_results.json"), "w") as fh:
        json.dump({"average_ms": avg}, fh)
    print(f"Average create latency: {avg:.2f}ms")


if __name__ == "__main__":
    main()
