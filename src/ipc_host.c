/* AOS — src/ipc_host.c — Purpose: C source file */
/*
 * AOS — ipc_host.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ipc_host.h"
#include "branch.h"
#include "ipc.h"
#include "ipc_protocol.h"
#include "logging.h"
#include "syscalls.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* Stub host daemon polling the IPC ring and logging requests */
void ipc_host_handle(IpcRing *ring) {
    if (ring->head == ring->tail)
        return;

    SyscallRequest *req = &ring->req[ring->tail % IPC_RING_SIZE];
    SyscallResponse *resp = &ring->resp[ring->tail % IPC_RING_SIZE];

    switch (req->id) {
    case SYS_AI_QUERY:
        log_message(LOG_INFO, "ai prompt %s", req->str_arg0);
        snprintf(resp->data, sizeof(resp->data), "OK");
        resp->retval = strlen(resp->data);
        break;
    case SYS_AI_INIT:
        log_message(LOG_INFO, "ai init %s", req->payload);
        resp->retval = 0;
        resp->data[0] = '\0';
        break;
    case SYS_AI_PROCESS:
        log_message(LOG_INFO, "ai process %d bytes", req->int_arg0);
        snprintf(resp->data, sizeof(resp->data), "processed");
        resp->retval = strlen(resp->data);
        break;
    case SYS_CREATE_BRANCH:
        resp->retval = sys_create_branch();
        break;
    case SYS_MERGE_BRANCH: {
        int rc = sys_merge_branch(req->branch_id);
        if (rc < 0) {
            snprintf(resp->data, sizeof(resp->data),
                     "{ \"error\": \"invalid branch\", \"code\": %d }", -rc);
            resp->retval = strlen(resp->data);
        } else {
            resp->retval = rc;
            resp->data[0] = '\0';
        }
        break;
    }
    case SYS_LIST_BRANCHES: {
        char buf[128];
        int rc = sys_list_branches(buf, sizeof(buf));
        if (rc < 0) {
            resp->retval = rc;
            resp->data[0] = '\0';
            break;
        }
        uint32_t count;
        memcpy(&count, buf, sizeof(count));
        size_t off = sizeof(count);
        int pos = snprintf(resp->data, sizeof(resp->data), "{ \"branches\": [");
        for (uint32_t i = 0; i < count && pos < (int)sizeof(resp->data); i++) {
            if (i)
                pos += snprintf(resp->data + pos, sizeof(resp->data) - pos, ", ");
            struct branch_info bi;
            if (off + sizeof(bi) > sizeof(buf))
                break;
            memcpy(&bi, buf + off, sizeof(bi));
            off += sizeof(bi);
            pos += snprintf(resp->data + pos, sizeof(resp->data) - pos,
                            "{ \"branch_id\": %u, \"parent_id\": %u, \"status\": %u, "
                            "\"last_snapshot_id\": %llu, \"owner_uid\": %u }",
                            bi.branch_id, bi.parent_id, bi.status,
                            (unsigned long long)bi.last_snapshot_id, bi.owner_uid);
        }
        if (pos < (int)sizeof(resp->data))
            pos += snprintf(resp->data + pos, sizeof(resp->data) - pos, "] }");
        resp->retval = pos;
        break;
    }
    case SYS_SNAPSHOT_BRANCH:
        *(uint64_t *)resp->data = sys_snapshot_branch((unsigned int)req->branch_id);
        resp->retval = 0;
        break;
    case SYS_DELETE_BRANCH: {
        int rc = sys_delete_branch((unsigned int)req->branch_id);
        if (rc < 0) {
            snprintf(resp->data, sizeof(resp->data),
                     "{ \"error\": \"invalid branch\", \"code\": %d }", -rc);
            resp->retval = strlen(resp->data);
        } else {
            resp->retval = 0;
            resp->data[0] = '\0';
        }
        break;
    }
    case SYS_FORK_BRANCH:
    case SYS_LIST_BRANCH:
        resp->retval = -ENOSYS;
        break;
    default:
        resp->retval = -1;
    }

    ring->tail++;
}

#ifndef IPC_HOST_LIBRARY
int main(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        return 1;
    }

    IpcRing *ring =
        mmap(NULL, sizeof(IpcRing), PROT_READ | PROT_WRITE, MAP_SHARED, fd, IPC_PHYS_ADDR);
    if (ring == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    log_message(LOG_INFO, "ipc_host mapped ring at %p", ring);
    bm_init();
    while (1) {
        ipc_host_handle(ring);
        usleep(1000);
    }
    return 0;
}
#endif
