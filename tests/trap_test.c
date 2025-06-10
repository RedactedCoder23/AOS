int main(void) {
    __asm__("mov $1, %eax\n\txor %eax, %eax\n\tdiv %eax");
    return 0;
}
