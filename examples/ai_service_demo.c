#include "ai.h"
int main(void){
    int id = ai_service_create("test","params");
    ai_service_monitor(id);
    return 0;
}
