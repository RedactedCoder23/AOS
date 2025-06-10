#include <stdint.h>

struct IdtEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

static struct IdtEntry idt[256];

static void set_gate(int vec, void (*handler)(void)) {
    uintptr_t addr = (uintptr_t)handler;
    idt[vec].offset_low = addr & 0xFFFF;
    idt[vec].selector = 0x08;
    idt[vec].zero = 0;
    idt[vec].type_attr = 0x8E;
    idt[vec].offset_high = (addr >> 16) & 0xFFFF;
}

static void page_fault_stub(void) {
    for (;;)
        __asm__("hlt");
}

void idt_init(void) {
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }
    set_gate(14, page_fault_stub);
    struct {
        uint16_t size;
        uint32_t addr;
    } __attribute__((packed)) idtr = {sizeof(idt) - 1, (uint32_t)idt};
    __asm__("lidt %0" : : "m"(idtr));
}
