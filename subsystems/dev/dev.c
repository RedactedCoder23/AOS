#include "dev.h"
#include "logging.h"

void driver_register(const char *name) {
    log_message(LOG_INFO, "driver %s registered (stub)", name);
}

void driver_list(void) { log_message(LOG_INFO, "no drivers registered"); }
