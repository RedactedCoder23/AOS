#include "traps.h"
#include <stdint.h>

static inline void outb(uint16_t p, uint8_t v) { asm volatile("outb %0,%1" ::"a"(v), "Nd"(p)); }
static inline uint8_t inb(uint16_t p) { uint8_t r; asm volatile("inb %1,%0" : "=a"(r) : "Nd"(p)); return r; }

static int serial_empty(void) { return inb(0x3F8 + 5) & 0x20; }
static void serial_write(char a) { while (!serial_empty()) ; outb(0x3F8, a); }
static void serial_print(const char *s) { while (*s) serial_write(*s++); }

static void print_hex(uint32_t v) {
    char hex[8];
    serial_print("0x");
    for (int i = 0; i < 8; i++) {
        int d = (v >> ((7 - i) * 4)) & 0xF;
        hex[i] = d < 10 ? '0' + d : 'a' + d - 10;
        serial_write(hex[i]);
    }
}

void isr_default(registers_t *r) {
    uint32_t cr2;
    __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
    serial_print("Page fault at ");
    print_hex(cr2);
    serial_print(" RIP=");
    print_hex(r->rip);
    serial_print(" RSP=");
    print_hex(r->rsp);
    serial_print("\n");
    for (;;)
        __asm__("hlt");
}

