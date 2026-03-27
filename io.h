#ifndef IO_H
#define IO_H

// Запись байта в порт (outb)
static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

// Чтение байта из порта (inb)
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

#endif
