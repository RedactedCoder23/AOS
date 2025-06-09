#include "error.h"
#include <stdio.h>

const char *aos_strerror(AosError err) {
    switch (err) {
    case AOS_OK:
        return "ok";
    case AOS_ERR_IO:
        return "io error";
    case AOS_ERR_NOMEM:
        return "out of memory";
    case AOS_ERR_INVALID:
        return "invalid argument";
    case AOS_ERR_LIMIT:
        return "limit reached";
    default:
        return "unknown";
    }
}
AosError aos_last_error = AOS_OK;
