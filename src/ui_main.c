/* AOS — src/ui_main.c — Purpose: C source file */
/*
 * AOS — ui_main.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ui_graph.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("AOS branch graph UI\n\n");
        printf("Controls:\n");
        printf("  Arrows/hjkl: move selection\n");
        printf("  Enter: switch branch\n");
        printf("  n: new branch\n");
        printf("  c: connect branches\n");
        printf("  q: quit\n");
        return 0;
    }
    return ui_graph_run();
}
