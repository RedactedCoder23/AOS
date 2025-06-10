#include "command_interpreter.h"
#include "config.h"
#include "error.h"
#include "logging.h"
#include "ipc.h"
#include <stdint.h>

/* Boot entry points provided by assembly stub. */
extern void repl(void);
void mem_init_bare(void);
void fs_init(void);
void bm_init(void);

/* Dispatch syscalls coming from the host interface */
extern char ipc_shared; /* linker symbol */
static volatile IpcRing *ring = (volatile IpcRing *)&ipc_shared;

static int syscall_dispatch(const SyscallRequest *req, SyscallResponse *resp) {
    (void)req;
    resp->retval = -1;
    return -1;
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
    mem_init_bare();
    fs_init();
    bm_init();
    config_load_default();
}

void main(void) {
    /* Entry called by bootloader. Start subsystems and drop into REPL. */
    kernel_init();
    process_ipc();
    repl();
}

void _start(void) {
    /* Minimal bootstrap that calls main and halts when it returns. */
    main();
    for (;;)
        __asm__("hlt");
}
