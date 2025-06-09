#include "branch.h"
int main(void){
    br_peer_add("127.0.0.1");
    br_sync();
    return 0;
}
