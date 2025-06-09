#include <stdio.h>

int plugin_init(void) {
    printf("sample plugin init\n");
    return 0;
}

void plugin_exec(void) {
    printf("sample plugin executed\n");
}

void plugin_cleanup(void) {
    printf("sample plugin cleanup\n");
}
