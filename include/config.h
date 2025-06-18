/* AOS — include/config.h — Purpose: Header file */
/*
 * AOS — config.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef CONFIG_H
#define CONFIG_H

/* Load key-value settings from /etc/aos/config.json */
void config_load_default(void);
/* Retrieve value for key, or NULL */
const char *config_get(const char *key);

#endif
