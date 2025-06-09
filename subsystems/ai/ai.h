#ifndef AI_H
#define AI_H

void ai_init(const char *profile);
const char *ai_reply(const char *prompt);
void ai_shutdown(void);

#endif
