# Configuration

## Autoscaler Environment Variables

The autoscaler daemon polls Redis and adjusts the number of worker
containers. Tune its behaviour using the following environment
variables:

| Variable | Description | Default |
|----------|-------------|---------|
| `AOS_RQ_MIN` | Minimum number of workers to keep running | `1` |
| `AOS_RQ_MAX` | Maximum number of workers allowed | `10` |
| `CHECK_INTERVAL` | Seconds between queue length checks | `5` |

Example `.env` fragment:

```env
AOS_RQ_MIN=2
AOS_RQ_MAX=8
CHECK_INTERVAL=10
```
