#ifndef AOS_ERROR_H
#define AOS_ERROR_H

/* Generic error codes used across subsystems */
typedef enum {
    AOS_OK = 0,
    AOS_ERR_IO = -1,
    AOS_ERR_NOMEM = -2,
    AOS_ERR_INVALID = -3,
    AOS_ERR_LIMIT = -4
} AosError;

const char *aos_strerror(AosError err);
extern AosError aos_last_error;

#endif
