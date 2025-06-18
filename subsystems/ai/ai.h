/* AOS — subsystems/ai/ai.h — Purpose: Header file */
/*
 * AOS — ai.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef AI_H
#define AI_H

void ai_init(const char *profile);
const char *ai_reply(const char *prompt);
void ai_shutdown(void);

int ai_infer(const char *prompt, char *out, unsigned long outsz);

int ai_config_load(void);
int ai_setup_wizard(void);
const char *ai_get_provider(void);
const char *ai_get_param(void);

int ai_service_create(const char *type, const char *params);
void ai_service_monitor(int id);

#endif
