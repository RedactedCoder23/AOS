#include "plugin.h"
int main(void){
    plugin_install("local");
    plugin_load("sample");
    plugin_list();
    return 0;
}
