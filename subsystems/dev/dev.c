/* AOS — subsystems/dev/dev.c — Purpose: C source file */
/*
 * AOS — dev.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "dev.h"
#include "logging.h"
#include <string.h>

typedef struct device {
    const char *name;
    int (*init)(void);
} Device;

static int serial_init(void);
static int serial_read(void *buf, int len);
static int serial_write(const void *buf, int len);

static int timer_init(void);
static int timer_read(void *buf, int len);
static int timer_write(const void *buf, int len);

static Device builtin[] = {
    {"serial", serial_init},
    {"timer", timer_init},
};

static const int builtin_count = sizeof(builtin) / sizeof(builtin[0]);

void driver_register(const char *name) {
    log_message(LOG_INFO, "driver %s registered", name);
}

void driver_list(void) {
    for (int i = 0; i < builtin_count; i++)
        log_message(LOG_INFO, "driver %s", builtin[i].name);
}

void dev_init(void) {
    for (int i = 0; i < builtin_count; i++) {
        int r = builtin[i].init();
        if (r == 0)
            log_message(LOG_INFO, "driver %s initialized", builtin[i].name);
        else
            log_message(LOG_ERROR, "driver %s init failed", builtin[i].name);
    }
}

int dev_read(const char *name, void *buf, int len) {
    if (strcmp(name, "serial") == 0)
        return serial_read(buf, len);
    if (strcmp(name, "timer") == 0)
        return timer_read(buf, len);
    log_message(LOG_ERROR, "unknown device %s", name);
    return -1;
}

int dev_write(const char *name, const void *buf, int len) {
    if (strcmp(name, "serial") == 0)
        return serial_write(buf, len);
    if (strcmp(name, "timer") == 0)
        return timer_write(buf, len);
    log_message(LOG_ERROR, "unknown device %s", name);
    return -1;
}

/* Serial driver */
static int serial_init(void) {
    log_message(LOG_INFO, "serial init");
    return 0;
}

static int serial_read(void *buf, int len) {
    (void)buf;
    (void)len;
    log_message(LOG_INFO, "serial read");
    return 0;
}

static int serial_write(const void *buf, int len) {
    (void)buf;
    log_message(LOG_INFO, "serial write");
    return len;
}

/* Timer driver */
static int timer_init(void) {
    log_message(LOG_INFO, "timer init");
    return 0;
}

static int timer_read(void *buf, int len) {
    (void)buf;
    (void)len;
    log_message(LOG_INFO, "timer read");
    return 0;
}

static int timer_write(const void *buf, int len) {
    (void)buf;
    log_message(LOG_INFO, "timer write");
    return len;
}
