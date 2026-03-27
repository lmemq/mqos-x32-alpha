#include "screen.h"
#include "input.h"
#include "idt.h"
#include "io.h"

extern void idt_install(); 
void keyboard_handler_main();

void kmain(void) {
    kclear();
    idt_install();
    inb(0x60); 
    asm volatile("sti");
    kprintl("Mq os: legacy x32 edition");
    kprintl("By lmemq");
    kprintl(" ");
    kprint("main#mq");
    kprint(" ");
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 12);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 14);
    while(1);
    return;
}

