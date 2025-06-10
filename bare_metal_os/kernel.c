#include "command_interpreter.h"
#include "config.h"
#include "error.h"
#include "idt.h"
#include "ipc.h"
#include "ipc_protocol.h"
#include "logging.h"
#include "traps.h"
#include "../include/syscalls.h"
#include <errno.h>
#include <stdint.h>

/* Boot entry points provided by assembly stub. */
extern void repl(void);
void mem_init_bare(void);
void fs_init(void);
void bm_init(void);
void idt_init(void);

/* Dispatch syscalls coming from the host interface */
extern char ipc_shared; /* linker symbol */
static volatile IpcRing *ring = (volatile IpcRing *)&ipc_shared;

static int syscall_dispatch(const SyscallRequest *req, SyscallResponse *resp) {
    resp->retval = -ENOSYS;
    switch (req->id) {
    case SYS_CREATE_BRANCH:
        resp->retval = sys_create_branch();
        break;
    case SYS_MERGE_BRANCH:
        resp->retval = sys_merge_branch(req->branch_id);
        break;
    case SYS_LIST_BRANCHES:
        resp->retval = sys_list_branches(resp->data, sizeof(resp->data));
        break;
    default:
        break;
    }
    return resp->retval;
}

static void process_ipc(void) {
    while (ring->head != ring->tail) {
        const SyscallRequest *req = &ring->req[ring->tail % IPC_RING_SIZE];
        SyscallResponse *resp = &ring->resp[ring->tail % IPC_RING_SIZE];
        syscall_dispatch(req, resp);
        ring->tail++;
    }
}

/* Initialise subsystems before entering the REPL. */
static void kernel_init(void) {
    log_init(NULL);
    log_message(LOG_INFO, "Hello from AOS");
    idt_init();
    mem_init_bare();
    fs_init();
    bm_init();
    config_load_default();
}

static void kernel_main(void) {
    idt_init();
    kernel_init();
    process_ipc();
    repl();
}

void main(void) { kernel_main(); }

void _start(void) {
    /* Minimal bootstrap that calls main and halts when it returns. */
    main();
    for (;;)
        __asm__("hlt");
}
