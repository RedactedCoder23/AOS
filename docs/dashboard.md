# Web Dashboard

A React and Vite application under `ui/` shows branch data and live metrics.
The backend exposes `/api/branches` and `/branches/<id>/metrics` endpoints.
A WebSocket endpoint streams status updates for running tasks. The dashboard
includes charts for CPU, memory and coverage history.
