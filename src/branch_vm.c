/*
 * AOS — branch_vm.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "branch.h"
#include <stdio.h>
#include <string.h>

#define MAX_VMS 4

typedef struct {
    int id;
    char name[32];
    char image[64];
    int mem;
    int cpu;
    char state[16];
} BranchVM;

static BranchVM vms[MAX_VMS];
static int vm_count;

int bm_vm_create(const char *name, const char *image, int mem, int cpu) {
    if (vm_count >= MAX_VMS)
        return -1;
    BranchVM *v = &vms[vm_count];
    v->id = vm_count;
    strncpy(v->name, name ? name : "vm", sizeof(v->name) - 1);
    strncpy(v->image, image ? image : "", sizeof(v->image) - 1);
    v->mem = mem;
    v->cpu = cpu;
    strncpy(v->state, "running", sizeof(v->state) - 1);
    printf("vm %d created (%s)\n", v->id, v->name);
    return vm_count++;
}

void bm_vm_list(void) {
    for (int i = 0; i < vm_count; i++)
        printf("%d:%s (%s)\n", vms[i].id, vms[i].name, vms[i].state);
}

int bm_vm_switch(int id) {
    if (id < 0 || id >= vm_count)
        return -1;
    printf("switched to vm %d\n", id);
    return 0;
}

int bm_vm_stop(int id) {
    if (id < 0 || id >= vm_count)
        return -1;
    strncpy(vms[id].state, "stopped", sizeof(vms[id].state) - 1);
    printf("stopped vm %d\n", id);
    return 0;
}
