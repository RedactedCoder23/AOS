# Profiler Stub

The current profiler records a simple elapsed time between `profiler_start()`
and `profiler_report()` calls.

```
profiler_start();
// code to measure
profiler_stop();
profiler_report("section A");
```

Future work should store results across multiple sections and integrate the
logging subsystem for persistent reporting.
