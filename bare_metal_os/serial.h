/*
 * AOS — serial.h
 * Purpose: Minimal 16550 UART driver
 * Dependencies: port I/O instructions
 */
#ifndef SERIAL_H
#define SERIAL_H
void serial_init(void);
void serial_write(char c);
void serial_print(const char *s);
#endif
