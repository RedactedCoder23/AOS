/* AOS — tests/lang_test.c — Purpose: C source file */
/*
 * AOS — lang_test.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "lang_vm.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *buf = NULL;
    size_t size = 0;
    FILE *mem = open_memstream(&buf, &size);
    FILE *orig = stdout;
    stdout = mem;
    int rc = lang_vm_run_file("examples/hello.aos");
    fflush(stdout);
    stdout = orig;
    fclose(mem);
    assert(rc == 0);
    assert(buf != NULL);
    assert(strcmp(buf, "Hello, AOS!\n") == 0);
    free(buf);
    printf("lang vm tests passed\n");
    return 0;
}
