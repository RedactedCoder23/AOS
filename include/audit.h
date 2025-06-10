#ifndef AOS_AUDIT_H
#define AOS_AUDIT_H

void audit_log_entry(const char *user, const char *action,
                     const char *resource, const char *result);

#endif
