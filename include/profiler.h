/* Minimal profiler stub. Future work should collect multiple samples and integrate with logging. */

#ifndef AOS_PROFILER_H
#define AOS_PROFILER_H

void profiler_start(void);
void profiler_stop(void);
void profiler_report(const char *label);

#endif
