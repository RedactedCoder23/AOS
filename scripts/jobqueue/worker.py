import redis
from rq import Worker, Queue, Connection


def main():
    redis_conn = redis.Redis()
    with Connection(redis_conn):
        worker = Worker([Queue()])
        worker.work()


if __name__ == "__main__":
    main()
