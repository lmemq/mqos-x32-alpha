#include "io.h" // Здесь должны быть ваши inb/outb

struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char  always0;
    unsigned char  flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed, aligned(4))); // Добавили выравнивание


struct idt_entry idt[256] __attribute__((aligned(16)));

struct idt_ptr idtp;

void dummy_handler() {
    outb(0x20, 0x20); // Master
    outb(0xA0, 0x20); // Slave (на всякий случай, если прилетит IRQ8+)
}


// Функция регистрации конкретного прерывания
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// Ремап PIC (обязательно для работы с клавиатурой в Multiboot)
void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // IRQ 0-7 -> прерывания 32-39
    outb(0xA1, 0x28); // IRQ 8-15 -> прерывания 40-47
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);  // Разрешить всё
    outb(0xA1, 0x0);
    outb(0x21, 0xFD); // 0xFD = 11111101 (разрешено только IRQ1 - клавиатура)
    outb(0xA1, 0xFF);
}

extern void dummy_handler_stub();
extern void keyboard_handler_stub();

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    // 1. Заполняем ВСЮ таблицу дефолтной заглушкой
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, (unsigned int)dummy_handler_stub, 0x08, 0x8E);
    }

    // 2. Делаем ремап PIC (обязательно!)
    pic_remap();

    // 3. Ставим конкретный обработчик для клавиатуры (IRQ1 -> 0x21)
    idt_set_gate(0x21, (unsigned int)keyboard_handler_stub, 0x08, 0x8E);

    // 4. Загружаем IDT
    asm volatile("lidt %0" : : "m"(idtp));
}

