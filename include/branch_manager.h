#ifndef BRANCH_MANAGER_H
#define BRANCH_MANAGER_H

/* Prototype branch/thread manager structs */
typedef struct thread_t {
    int placeholder;
} thread_t;

typedef struct branch_t {
    thread_t *thread;
    struct branch_t *parent;
} branch_t;

branch_t *branch_create(void (*entry)(void *), void *arg);
branch_t *branch_fork(branch_t *parent);
void branch_join(branch_t *b);

#endif /* BRANCH_MANAGER_H */
