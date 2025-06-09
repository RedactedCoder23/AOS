#include "logging.h"

void ai_trigger(const char *prompt) {
    (void)prompt;
    log_message(LOG_INFO, "AI_TRIGGER invoked");
}
