#include <stdlib.h>
#include <string.h>
#include "utils.h"

char *reverse(const char *input) {
    if (!input) return NULL;
    size_t len = strlen(input);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    for (size_t i = 0; i < len; i++) {
        out[i] = input[len - i - 1];
    }
    out[len] = '\0';
    return out;
}
