#include "ipc.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAX_ENTRIES 16

typedef struct {
    int id;
    char name[32];
} Entry;

static IpcRing *ring;

static int ipc_init(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0)
        return -1;
    ring = mmap(NULL, sizeof(IpcRing), PROT_READ | PROT_WRITE, MAP_SHARED, fd, IPC_PHYS_ADDR);
    close(fd);
    return ring == MAP_FAILED ? -1 : 0;
}

static void ipc_send(const SyscallRequest *req, SyscallResponse *resp) {
    size_t idx = ring->head % IPC_RING_SIZE;
    ring->req[idx] = *req;
    __sync_synchronize();
    ring->head++;
    while (ring->tail <= idx)
        usleep(1000);
    if (resp)
        *resp = ring->resp[idx];
}

static int fetch_entries(Entry *out) {
    SyscallRequest req = {0};
    SyscallResponse resp = {0};
    req.id = SYS_LIST_BRANCH;
    ipc_send(&req, &resp);
    int count = 0;
    char *tok = strtok(resp.data, ";");
    while (tok && count < MAX_ENTRIES) {
        sscanf(tok, "%d:%31s", &out[count].id, out[count].name);
        tok = strtok(NULL, ";");
        count++;
    }
    return count;
}

int main(void) {
    if (ipc_init() < 0) {
        fprintf(stderr, "failed to map ipc page\n");
        return 1;
    }
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    Entry entries[MAX_ENTRIES];
    int selected = 0;
    char status[80] = "";
    int running = 1;
    while (running) {
        int count = fetch_entries(entries);
        clear();
        for (int i = 0; i < count; i++) {
            if (i == selected)
                attron(A_REVERSE);
            mvprintw(i, 0, "%d:%s", entries[i].id, entries[i].name);
            if (i == selected)
                attroff(A_REVERSE);
        }
        mvprintw(LINES - 2, 0, "c=create  j=join  k=kill  q=quit");
        mvprintw(LINES - 1, 0, "%s", status);
        refresh();
        int ch = getch();
        SyscallRequest req = {0};
        SyscallResponse resp = {0};
        switch (ch) {
        case 'c':
            echo();
            mvprintw(LINES - 1, 0, "name: ");
            char name[32];
            getnstr(name, sizeof(name) - 1);
            noecho();
            req.id = SYS_FORK_BRANCH;
            req.int_arg0 = -1;
            strncpy(req.str_arg0, name, sizeof(req.str_arg0) - 1);
            ipc_send(&req, &resp);
            snprintf(status, sizeof(status), "create %s -> %d", name, resp.retval);
            break;
        case 'j':
            req.id = SYS_MERGE_BRANCH;
            req.int_arg0 = entries[selected].id;
            req.int_arg1 = 0;
            ipc_send(&req, &resp);
            snprintf(status, sizeof(status), "join %d -> %d", entries[selected].id, resp.retval);
            break;
        case 'k':
            req.id = SYS_DELETE_BRANCH;
            req.int_arg0 = entries[selected].id;
            ipc_send(&req, &resp);
            snprintf(status, sizeof(status), "kill %d -> %d", entries[selected].id, resp.retval);
            if (selected > 0)
                selected--;
            break;
        case KEY_DOWN:
            if (selected < count - 1)
                selected++;
            break;
        case KEY_UP:
            if (selected > 0)
                selected--;
            break;
        case 'q':
            running = 0;
            break;
        }
    }
    endwin();
    return 0;
}
