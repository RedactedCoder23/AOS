#include "plugin.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    assert(plugin_load("sample") == 0);
    assert(plugin_exec("sample") == 0);
    assert(plugin_unload("sample") == 0);
    printf("plugin tests passed\n");
    return 0;
}
