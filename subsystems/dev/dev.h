/*
 * AOS — dev.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef AOS_DEV_H
#define AOS_DEV_H

typedef struct device {
    const char *name;
    int (*init)(void);
} Device;

void driver_register(const char *name);
void driver_list(void);
void dev_init(void);
int dev_read(const char *name, void *buf, int len);
int dev_write(const char *name, const void *buf, int len);
#endif

