#ifndef AOS_LOGGING_H
#define AOS_LOGGING_H

#include <stdio.h>

typedef enum { LOG_INFO, LOG_WARN, LOG_ERROR } LogLevel;

void log_init(FILE *dest);
void log_message(LogLevel level, const char *fmt, ...);

#endif
