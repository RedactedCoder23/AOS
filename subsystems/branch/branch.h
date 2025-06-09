#ifndef BRANCH_H
#define BRANCH_H

void branch_init();
int branch_create(const char *name);
void branch_list();
int branch_stop(int id);
int branch_delete(int id);

#endif
