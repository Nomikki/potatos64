# Makefile

PREFIX=i686-elf
CC=$(PREFIX)-gcc
LD=$(PREFIX)-ld
CFLAGS=-ffreestanding -m32 -g
LDFLAGS=-nostdlib -no-pie -Ttext 0x100000

QEMU_SYSTEM := qemu-system-x86_64.exe

ASM=nasm
ASMFLAGS=-f elf

BUILDDIR=build
ISODIR=$(BUILDDIR)/iso
ISO_FILENAME = $(BUILDDIR)/potatos.iso

all: clean iso run

$(BUILDDIR)/boot.o: boot/boot.asm
	@mkdir -p $(BUILDDIR)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILDDIR)/gdt.o: kernel/gdt.asm
	@mkdir -p $(BUILDDIR)
	$(ASM) $(ASMFLAGS) -o $@ $<


$(BUILDDIR)/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/kernel.elf: $(BUILDDIR)/boot.o $(BUILDDIR)/gdt.o  $(BUILDDIR)/kernel.o
	$(LD) $(LDFLAGS) -o $@ $^

iso: $(BUILDDIR)/kernel.elf
	@mkdir -p $(ISODIR)/boot/grub
	cp $(BUILDDIR)/kernel.elf $(ISODIR)/boot/
	cp grub/grub.cfg $(ISODIR)/boot/grub/
	grub-mkrescue -o $(BUILDDIR)/potatos.iso $(ISODIR)

clean:
	rm -rf $(BUILDDIR)

run:
# qemu-system-x86_64.exe -cdrom $(ISO_FILENAME) -serial file:"serial.log"
	$(QEMU_SYSTEM) -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(ISO_FILENAME) -serial file:"serial.log" -m 1G -no-reboot -no-shutdown