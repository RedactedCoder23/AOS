#ifndef POLICY_H
#define POLICY_H

/* Load policy rules from JSON string */
void policy_load(const char *json);
/* Check action against current policy for a branch/app */
int policy_check_ctx(const char *branch, const char *app, const char *action);
/* Backwards compatibility helper using default context */
int policy_check(const char *action);

#endif
