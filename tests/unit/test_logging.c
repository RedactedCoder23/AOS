#include "logging.h"
#include <stdio.h>
int main(void) {
    log_init(stdout);
    log_message(LOG_INFO, "log ok");
    log_message(LOG_WARN, "warn ok");
    log_message(LOG_ERROR, "error ok");
    return 0;
}
