# Переменные для удобства
CC = gcc
AS = nasm
LD = ld

# Флаги компиляции
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib
LDFLAGS = -m elf_i386 -T link.ld -z max-page-size=0x1000
ASFLAGS = -f elf32

# Список объектных файлов (добавляй сюда новые .o по мере роста ОС)
OBJ = kasm.o kernel.o screen.o idt.o input.o gdt.o

# Главная цель
all: mqos

# Добавьте в начало к переменным
MKRESCUE = grub-mkrescue
ISO_DIR = isodir
ISO_NAME = mqos.iso

# Новая цель для создания ISO
iso: all
	mkdir -p $(ISO_DIR)/boot/grub
	cp mqos $(ISO_DIR)/boot/mqos # Копируем ядро в /boot/mqos
	
	# Создаем конфиг GRUB
	@echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "My OS" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  multiboot /boot/mqos' >> $(ISO_DIR)/boot/grub/grub.cfg # ПУТЬ ДОЛЖЕН СОВПАДАТЬ!
	@echo '  boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)
	rm -rf $(ISO_DIR)

# Правило сборки ядра
mqos: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o mqos

# Правило для .c файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для .asm файлов
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Запуск в QEMU
run: all
	qemu-system-i386 -kernel mqos

# Очистка проекта

# Обновите clean, чтобы удалял и ISO
clean:
	rm -f *.o mqos $(ISO_NAME)
	rm -rf $(ISO_DIR)

