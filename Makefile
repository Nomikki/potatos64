# Makefile

PREFIX=x86_64-elf
CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
ASM=nasm

#CFLAGS=-ffreestanding -m64 -g
CFLAGS= -std=gnu99 \
				-nostdlib \
				-fno-builtin \
				-fno-exceptions \
				-fno-leading-underscore \
				-Wno-write-strings \
				-fno-pic \
				-ffreestanding \
        -ffreestanding \
        -Wall \
        -Wextra \
        -Iinclude \
        -mno-red-zone \
        -mno-sse \
        -mcmodel=large \
				-Iinclude

LDFLAGS=-melf_x86_64
ASMFLAGS=-f elf64
QEMU_SYSTEM := qemu-system-x86_64.exe

BUILDDIR=build
OBJECTDIR=obj
ISODIR=$(BUILDDIR)/iso
ISO_FILENAME = $(BUILDDIR)/potatos.iso
FONTNAME = font.psf

OBJECTS = obj/boot/boot.o \
					obj/drivers/framebuffer/framebuffer.o \
					obj/drivers/keyboard.o \
					obj/drivers/ports.o \
					obj/drivers/serial.o \
					obj/drivers/vga.o \
					obj/kernel/kernel.o \
					obj/lib/stdlib.o \
					obj/lib/stdio.o \
					obj/lib/string.o \
					obj/arch/x86_64/idt/idt.o \
					obj/arch/x86_64/idt/idt_stub.o \
					obj/mem/mmap.o \
					obj/mem/pmm.o \
					obj/mem/bitmap.o \
					obj/mem/paging.o \
					obj/mem/vmm.o \
					obj/mem/kheap.o \
					obj/scheluding/scheluder.o \
					obj/utils/hexdump.o 

all: clean iso createFont run

obj/%.o: src/%.asm
		@mkdir -p $(@D)
		nasm $(ASMFLAGS) $< -o $@


obj/%.o: src/%.c
		@mkdir -p $(@D)
		x86_64-elf-gcc $(CFLAGS) -o $@ -c $< 


$(BUILDDIR)/kernel.elf: linker.ld $(OBJECTS)
	@mkdir -p $(ISODIR)/boot/grub
	$(LD) -n -o $(LDFLAGS) -T $< -o $@ $(OBJECTS) obj/font.o

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(OBJECTDIR)


iso: createFont $(BUILDDIR)/kernel.elf
	@mkdir -p $(BUILDDIR)
	cp $(BUILDDIR)/kernel.elf $(ISODIR)/boot/
	cp grub/grub.cfg $(ISODIR)/boot/grub/
	grub-mkrescue -o $(BUILDDIR)/potatos.iso $(ISODIR)


createFont:
	mkdir -p obj
	objcopy -O elf64-x86-64 -B i386 -I binary $(FONTNAME) obj/font.o
	readelf -s obj/font.o

run:
# qemu-system-x86_64.exe -cdrom $(ISO_FILENAME) -serial file:"serial.log"
	
	$(QEMU_SYSTEM) -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(ISO_FILENAME) -serial file:"serial.log" -m 128M -no-reboot -no-shutdown