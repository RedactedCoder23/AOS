/* AOS — apps_src/textedit.c — Purpose: C source file */
/*
 * AOS — textedit.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: textedit <file>\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "w");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, ".quit\n") == 0)
            break;
        fputs(line, f);
    }
    fclose(f);
    return 0;
}
