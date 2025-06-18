/* AOS — tests/trap_test.c — Purpose: C source file */
/*
 * AOS — trap_test.c
 * (c) 2025 RedactedCoder23
 * Purpose: Source file.
 */
int main(void) {
    __asm__("mov $1, %eax\n\txor %eax, %eax\n\tdiv %eax");
    return 0;
}
