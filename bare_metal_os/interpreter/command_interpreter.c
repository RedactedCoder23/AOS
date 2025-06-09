#include "command_interpreter.h"
#include <stdint.h>

/* basic serial helpers */
static inline void outb(uint16_t p, uint8_t v){asm volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline uint8_t inb(uint16_t p){uint8_t r;asm volatile("inb %1,%0":"=a"(r):"Nd"(p));return r;}
static void serial_init(void){
    outb(0x3F8+1,0x00);
    outb(0x3F8+3,0x80);
    outb(0x3F8+0,0x03);
    outb(0x3F8+1,0x00);
    outb(0x3F8+3,0x03);
    outb(0x3F8+2,0xC7);
    outb(0x3F8+4,0x0B);
}
static int serial_received(void){return inb(0x3F8+5)&1;}
static char serial_read(void){while(!serial_received());return inb(0x3F8);}
static int serial_empty(void){return inb(0x3F8+5)&0x20;}
static void serial_write(char a){while(!serial_empty());outb(0x3F8,a);}
static void serial_print(const char *s){while(*s)serial_write(*s++);}

static int str_cmp(const char *a,const char *b){while(*a&&*a==*b){a++;b++;}return (unsigned char)*a-(unsigned char)*b;}

/* command registration and stubs so generated tables link */
void add_command(const char *name, command_handler handler){(void)name;(void)handler;}
#define STUB(fn) void fn(int a,char **b){(void)a;(void)b;}
STUB(cmd_hello_wrapper)
STUB(cmd_bye_wrapper)
STUB(cmd_ui_graph_wrapper)
STUB(cmd_mem_alloc_wrapper)
STUB(cmd_mem_free_wrapper)
STUB(cmd_fs_open_wrapper)
STUB(cmd_fs_write_wrapper)
STUB(cmd_fs_read_wrapper)
STUB(cmd_fs_close_wrapper)
STUB(cmd_fs_ls_wrapper)
STUB(cmd_ai_prompt_wrapper)
STUB(cmd_ai_profile_wrapper)
STUB(cmd_br_create_wrapper)
STUB(cmd_br_list_wrapper)
STUB(cmd_br_stop_wrapper)
STUB(cmd_br_delete_wrapper)
STUB(cmd_br_vm_create_wrapper)
STUB(cmd_br_vm_list_wrapper)
STUB(cmd_br_vm_switch_wrapper)
STUB(cmd_br_vm_stop_wrapper)
STUB(cmd_plugin_install_wrapper)
STUB(cmd_plugin_list_wrapper)
STUB(cmd_plugin_load_wrapper)
STUB(cmd_br_peer_add_wrapper)
STUB(cmd_br_sync_wrapper)
STUB(cmd_br_sync_all_wrapper)
STUB(cmd_ai_service_create_wrapper)
STUB(cmd_ai_service_monitor_wrapper)
STUB(cmd_policy_load_wrapper)
STUB(cmd_policy_check_wrapper)

static void readline(char *buf,int max){
    int i=0;char c;
    while(i<max-1){
        c=serial_read();
        if(c=='\r')continue;
        if(c=='\n'){serial_write('\n');break;}
        serial_write(c);
        buf[i++]=c;
    }
    buf[i]=0;
}

void repl(void){
    serial_init();
    char line[128];
    while(1){
        serial_print("bare> ");
        readline(line,sizeof(line));
        if(str_cmp(line,"exit")==0)break;
        serial_print("ECHO: ");
        serial_print(line);
        serial_print("\n");
    }
}

/* main for possible standalone testing; kernel.c provides its own entry */
#ifdef BARE_METAL_STANDALONE
int main(void){
    repl();
    return 0;
}
#endif
