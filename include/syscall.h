/* AOS — include/syscall.h — Purpose: Header file */
/*
 * AOS — syscall.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef SYSCALL_H
#define SYSCALL_H

#include "ipc.h"
#include <stddef.h>

void syscall_init(IpcRing *shared);
int sys_ai_query(const char *prompt, char *out, size_t outsz);

#endif /* SYSCALL_H */
