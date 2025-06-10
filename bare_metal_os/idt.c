#include <stdint.h>
<<<<<< codex/implement-default-trap/exception-handler
#include "idt.h"
#include "traps.h"

typedef struct {
=======

struct IdtEntry {
>>>>>> main
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
<<<<<< codex/implement-default-trap/exception-handler
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[256];
static idt_ptr_t idt_desc;

static void idt_set_gate(int n, void (*handler)(void)) {
    uint32_t addr = (uint32_t)handler;
    idt[n].offset_low = addr & 0xFFFF;
    idt[n].selector = 0x08; /* code segment */
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; /* present, dpl=0, 32-bit interrupt gate */
    idt[n].offset_high = (addr >> 16) & 0xFFFF;
=======
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
>>>>>> main
}

void idt_init(void) {
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }
<<<<<< codex/implement-default-trap/exception-handler

    idt_set_gate(14, (void (*)(void))isr_default);
    idt_set_gate(0x80, (void (*)(void))isr_default);

    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base = (uint32_t)&idt;

    __asm__ volatile("lidt %0" : : "m"(idt_desc));
}

=======
    set_gate(14, page_fault_stub);
    struct {
        uint16_t size;
        uint32_t addr;
    } __attribute__((packed)) idtr = {sizeof(idt) - 1, (uint32_t)idt};
    __asm__("lidt %0" : : "m"(idtr));
}
>>>>>> main
