#include "screen.h"
#include "io.h"

char *vidptr = (char*)0xb8000;
unsigned int vpc = 0; // video pointer (current)

void update_cursor() {
    unsigned short pos = vpc / 2; // vpc — это байтовое смещение, делим на 2 для индекса символа
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char) ((pos >> 8) & 0xFF));
}

void kclear(void) {
    while(vpc < 80 * 25 * 2) {
        vidptr[vpc++] = ' ';
        vidptr[vpc++] = 0x07;    
    }
    vpc = 0;
    return;
}

void kprint(const char *text) {
    unsigned int i = 0;
    while(text[i] != '\0') {
        vidptr[vpc++] = text[i++]; 
        vidptr[vpc++] = 0x07;       
    }
    update_cursor();
    return;
}

void kprintl(const char *text) {
    kprint(text);
    vpc += 160 - vpc % 160;
    update_cursor();
}

// if (vpc / 160 >= 24) {

// }
