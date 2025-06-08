#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int main(void) {
    char *rev = reverse("hello");
    assert(strcmp(rev, "olleh") == 0);
    free(rev);

    rev = reverse("");
    assert(strcmp(rev, "") == 0);
    free(rev);

    rev = reverse("a");
    assert(strcmp(rev, "a") == 0);
    free(rev);

    return 0;
}
