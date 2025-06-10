#include "security.h"
#include <assert.h>
int main(void) {
    security_init();
    assert(check_capability("none") == 0);
    return 0;
}
