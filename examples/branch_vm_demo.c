/*
 * AOS — branch_vm_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "branch.h"
int main(void) {
    bm_vm_create("demo", "image", 512, 1);
    bm_vm_list();
    bm_vm_stop(0);
    bm_vm_list();
    return 0;
}
