/* AOS — subsystems/security/security.h — Purpose: Header file */
/*
 * AOS — security.h
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#ifndef AOS_SECURITY_H
#define AOS_SECURITY_H

void security_init(void);
int check_capability(const char *cap);
int security_check(int syscall_id);

#endif

