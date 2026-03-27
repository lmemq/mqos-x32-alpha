#include "input.h"
#include "io.h"
#include "screen.h"

extern unsigned int vpc;
volatile char last_key = 0;

unsigned int pre_vpc = 0;

char cmd_now[1024];
unsigned int cmd_now_ptr = 0;

void pre() {
    kprint("main#mq ");
    pre_vpc = vpc;
}

unsigned char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   /* 0x00 - 0x0E */
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    /* 0x0F - 0x1C */
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,         /* 0x1D - 0x2A (0=LShift) */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,                 /* 0x2B - 0x36 (0=RShift) */
    '*', 0, ' ', 0,                                                            /* 0x37 - 0x3A (0=LAlt, 0=CapsLock) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                              /* 0x3B - 0x44 (F1 - F10) */
    0, 0,                                                                      /* 0x45 - 0x46 (NumLock, ScrollLock) */
    0, 0, 0, '-',                                                              /* 0x47 - 0x4A (Home, Up, PgUp, -) */
    0, 0, 0, '+',                                                              /* 0x4B - 0x4E (Left, Center, Right, +) */
    0, 0, 0, 0, 0, 0, 0,                                                       /* 0x4F - 0x55 (End, Down, PgDn, Ins, Del...) */
    0, 0, 0, 0, 0                                                              /* 0x56 - 0x58 (F11, F12) */
    /* Остальные заполняются нулями */
};

char scancode_to_ascii(unsigned char scancode) {
    // Игнорируем отпускание (bit 7 set)
    if (scancode & 0x80) return 0;
    
    // Проверяем, что скан-код не выходит за пределы нашего массива
    if (scancode >= 128) return 0;

    return kbd_us[scancode];
}

void keyboard_handler_main() {
    unsigned char status = inb(0x64);
    
    // Проверяем, что в буфере реально есть данные
    if (status & 0x01) {
        unsigned char scancode = inb(0x60);
        
        // Превращаем скан-код в символ и кладем в очередь/переменную
        last_key = scancode_to_ascii(scancode);
    }
    else {
        outb(0x20, 0x20); 
        return;
    }
    
    if (last_key == 0) {
        outb(0x20, 0x20); 
        return; 
    }
    
    if (last_key == '\b') {
        if (vpc < 2 || vpc > pre_vpc) {
            outb(0x20, 0x20); 
            return;
        }
    	vpc -= 2;
    	kprint(" ");
    	vpc -= 2;
        update_cursor(); 
    	outb(0x20, 0x20); 
    	return;
    }
    
    if (last_key == '\n') {
        // send cmd
        kprintl("");
        kprintl(cmd_now);
        cmd_now_ptr = 0;
        cmd_now[0] = '\0';
        pre();
        outb(0x20, 0x20);
        return;
    }

    if (cmd_now_ptr < 1023) {
        cmd_now[cmd_now_ptr++] = last_key;
        cmd_now[cmd_now_ptr] = '\0';
    }
    
    char str[2] = {last_key, '\0'};
    
    kprint(str);
    
    outb(0x20, 0x20); 
}
