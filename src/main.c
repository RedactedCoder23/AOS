#include <stdio.h>
#include <stdlib.h>
#include "branch.h"
#include "ai.h"
#include "plugin.h"
#include "fs.h"
#include "app_runtime.h"
#include "config.h"
#include "logging.h"
#include "repl.h"

int main(void) {
    FILE *logf = fopen("AOS-CHECKLIST.log", "a");
    log_init(logf);

    bm_init();
    config_load_default();
    fs_init();
    if (ai_config_load() != 0)
        printf("Run 'ai setup' to configure provider\n");
    const char *p = getenv("AOS_PORT");
    if (p) br_set_port(atoi(p));
    br_start_service();
    branch_load_all();

    repl_run();

    if (logf) fclose(logf);
    return 0;
}
