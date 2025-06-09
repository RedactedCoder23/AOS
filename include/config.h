#ifndef CONFIG_H
#define CONFIG_H

/* Load key-value settings from /etc/aos/config.json */
void config_load_default(void);
/* Retrieve value for key, or NULL */
const char *config_get(const char *key);

#endif
