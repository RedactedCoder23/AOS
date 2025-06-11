#!/usr/bin/env bash
set -e

docker-compose up -d
sleep 5
curl -fs http://localhost:8080 || exit 1
curl -fs http://localhost:5000/branches || exit 1
docker-compose down

echo "Demo smoke test passed"
