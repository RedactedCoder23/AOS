#include "fs.h"
#include <stdio.h>

int main(void){
    fs_init();
    fs_write("hello.txt","world");
    fs_write("aos.txt","demo");
    fs_list();
    printf("read aos.txt=%s\n", fs_read("aos.txt"));
    return 0;
}
