#include <stdint.h>

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
}
static void serial_write(char c) {
    while (!(inb(0x3F8 + 5) & 0x20));
    outb(0x3F8, c);
}

void kputs(const char *s) {
    volatile uint16_t *pos = VGA;
    for (; *s; s++) {
        *pos++ = 0x1F00 | *s;
        serial_write(*s);
    }
}

void main(void) {
    serial_init();
    kputs("Kernel started\n");
}

void _start(void) {
    main();
    for (;;) __asm__("hlt");
}
