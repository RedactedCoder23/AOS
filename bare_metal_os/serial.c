/*
 * AOS — serial.c
 * Purpose: Basic UART routines for early boot
 * Dependencies: serial.h
 */
#include "serial.h"
#include <stdint.h>

static inline void outb(uint16_t p, uint8_t v){asm volatile("outb %0,%1"::"a"(v),"Nd"(p));}
static inline uint8_t inb(uint16_t p){uint8_t r;asm volatile("inb %1,%0":"=a"(r):"Nd"(p));return r;}

void serial_init(void){
    outb(0x3F8 + 1,0x00);
    outb(0x3F8 + 3,0x80);
    outb(0x3F8 + 0,0x03);
    outb(0x3F8 + 1,0x00);
    outb(0x3F8 + 3,0x03);
    outb(0x3F8 + 2,0xC7);
    outb(0x3F8 + 4,0x0B);
}

static int serial_empty(void){return inb(0x3F8 + 5) & 0x20;}

void serial_write(char c){
    while(!serial_empty());
    outb(0x3F8,c);
}

void serial_print(const char *s){while(*s)serial_write(*s++);} 
