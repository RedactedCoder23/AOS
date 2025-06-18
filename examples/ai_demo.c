/* AOS — examples/ai_demo.c — Purpose: C source file */
/*
 * AOS — ai_demo.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
#include "ai.h"
#include <stdio.h>
#include <string.h>
int main(void) {
    ai_init("default");
    char prompt[64];
    if (!fgets(prompt, sizeof(prompt), stdin))
        return 1;
    char *nl = strchr(prompt, '\n');
    if (nl)
        *nl = '\0';
    printf("AI says: %s\n", ai_reply(prompt));
    ai_shutdown();
    return 0;
}
