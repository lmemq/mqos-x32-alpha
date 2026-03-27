bits 32

section .multiboot
    align 4
    dd 0x1BADB002        
    dd 0x00000007        ; Флаги те же
    dd -(0x1BADB002 + 0x00000007) 

    ; Эти поля обязательны, если стоит флаг 0x04 (Video)
    dd 0, 0, 0, 0, 0     ; Адреса (пропускаем для ELF)
    
    dd 1                 ; mode_type: 1 = ТЕКСТОВЫЙ РЕЖИМ (VGA)
    dd 80                ; ширина
    dd 25                ; высота
    dd 0                 ; глубина (для текста не важна)

section .text
extern gdt_install  ; Сообщаем ассемблеру, что функция в другом файле
extern idt_install
extern kmain

global start
start:
    cli
    mov esp, stack_top

    call gdt_install   ; Вызываем Си-функцию настройки таблицы
    
    jmp 0x08:.reload_cs ; Делаем Far Jump для обновления CS

.reload_cs:
    mov ax, 0x10       ; 0x10 — это смещение сегмента данных в нашей GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call idt_install   ; Теперь ставим IDT
    sti                ; ВКЛЮЧАЕМ прерывания
    
    call kmain         ; Прыгаем в ядро
    jmp $


; --- Обработчики ---

[extern keyboard_handler_main]
global keyboard_handler_stub

keyboard_handler_stub:
    pushad          ; Сохраняем все регистры общего назначения
    cld             ; Сбрасываем флаг направления (важно для Си)

    call keyboard_handler_main

    ; Отправляем сигнал окончания прерывания (EOI) в Master PIC
    mov al, 0x20
    out 0x20, al

    popad           ; Восстанавливаем регистры
    iretd           ; Возвращаемся из прерывания


global dummy_handler_stub
dummy_handler_stub:
    pushad          ; Сохраняем все регистры (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
    
    ; Отправляем EOI (End of Interrupt) в PIC, чтобы он знал, что мы закончили
    mov al, 0x20
    out 0x20, al    ; Master PIC
    out 0xA0, al    ; Slave PIC
    
    popad           ; Восстанавливаем регистры
    iretd           ; Возврат из прерывания (обязательно iretd для 32 бит!)


section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

