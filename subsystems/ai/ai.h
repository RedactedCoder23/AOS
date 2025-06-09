#ifndef AI_H
#define AI_H

void ai_init(const char *profile);
const char *ai_reply(const char *prompt);
void ai_shutdown(void);

int ai_service_create(const char *type, const char *params);
void ai_service_monitor(int id);

#endif
