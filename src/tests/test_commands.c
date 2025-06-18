/* AOS — src/tests/test_commands.c — Purpose: C source file */
/*
 * AOS — test_commands.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ai.h"
#include "branch.h"
#include "command_interpreter.h"
#include "fs.h"
#include "interpreter.h"
#include "memory.h"
#include "plugin.h"
#include "policy.h"
#include "repl.h"
#include <assert.h>
#include <stdio.h>

extern CommandDictionary commands[];

int ui_graph_run(void) { return 0; }

int main(void) {
    unsigned char mem[1024];
    memory_init(mem, sizeof(mem));
    fs_init();
    ai_init("default");
    bm_init();
    init_command_map();

    char *a1[] = {"HELLO", NULL};
    assert(dispatch_command(1, a1) == 0);
    char *a2[] = {"BYE", NULL};
    assert(dispatch_command(1, a2) == 0);
    char *a3[] = {"UI_GRAPH", NULL};
    assert(dispatch_command(1, a3) == 0);
    char *a4[] = {"MEM_ALLOC", "16", NULL};
    assert(dispatch_command(2, a4) == 0);
    char *a5[] = {"MEM_FREE", "0", NULL};
    assert(dispatch_command(2, a5) == 0);
    char *a6[] = {"FS_OPEN", "f", "w", NULL};
    assert(dispatch_command(3, a6) == 0);
    char *a7[] = {"FS_WRITE", "0", "hi", NULL};
    assert(dispatch_command(3, a7) == 0);
    char *a8[] = {"FS_READ", "0", "2", NULL};
    assert(dispatch_command(3, a8) == 0);
    char *a9[] = {"FS_CLOSE", "0", NULL};
    assert(dispatch_command(2, a9) == 0);
    char *a10[] = {"FS_MKDIR", "d", NULL};
    assert(dispatch_command(2, a10) == 0);
    char *a11[] = {"FS_LS", NULL};
    assert(dispatch_command(1, a11) == 0);
    char *a12[] = {"AI_PROMPT", "hi", NULL};
    assert(dispatch_command(2, a12) == 0);
    char *a13[] = {"AI_PROFILE", "default", NULL};
    assert(dispatch_command(2, a13) == 0);
    char *a14[] = {"BR_CREATE", "b1", NULL};
    assert(dispatch_command(2, a14) == 0);
    char *a15[] = {"BR_LIST", NULL};
    assert(dispatch_command(1, a15) == 0);
    char *a16[] = {"BR_STOP", "0", NULL};
    assert(dispatch_command(2, a16) == 0);
    char *a17[] = {"BR_DELETE", "0", NULL};
    assert(dispatch_command(2, a17) == 0);
    char *a18[] = {"BR_VM_CREATE", "vm", NULL};
    assert(dispatch_command(2, a18) == 0);
    char *a19[] = {"BR_VM_LIST", NULL};
    assert(dispatch_command(1, a19) == 0);
    char *a20[] = {"BR_VM_SWITCH", "0", NULL};
    assert(dispatch_command(2, a20) == 0);
    char *a21[] = {"BR_VM_STOP", "0", NULL};
    assert(dispatch_command(2, a21) == 0);
    char *a22[] = {"PLUGIN_INSTALL", "url", NULL};
    assert(dispatch_command(2, a22) == 0);
    char *a23[] = {"PLUGIN_LIST", NULL};
    assert(dispatch_command(1, a23) == 0);
    char *a24[] = {"PLUGIN_LOAD", "none", NULL};
    assert(dispatch_command(2, a24) == 0);
    char *a25[] = {"BR_PEER_ADD", "127.0.0.1", NULL};
    assert(dispatch_command(2, a25) == 0);
    char *a26[] = {"BR_SYNC", NULL};
    assert(dispatch_command(1, a26) == 0);
    char *a27[] = {"BR_SYNC_ALL", NULL};
    assert(dispatch_command(1, a27) == 0);
    char *a28[] = {"AI_SERVICE_CREATE", NULL};
    assert(dispatch_command(1, a28) == 0);
    char *a29[] = {"AI_SERVICE_MONITOR", NULL};
    assert(dispatch_command(1, a29) == 0);
    char *a30[] = {"POLICY_LOAD", "file", NULL};
    assert(dispatch_command(2, a30) == 0);
    char *a31[] = {"POLICY_CHECK", "act", NULL};
    assert(dispatch_command(2, a31) == 0);

    printf("command handler unit tests passed\n");
    return 0;
}
