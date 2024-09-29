#include <arch/x86_64/idt/idt.h>
#include <stdio.h>
#include <string.h>

struct interrupt_descriptor idt[IDT_SIZE];

void idt_set_entry(uint8_t vector, void *handler, uint8_t dpl)
{
  uint64_t handler_addr = (uint64_t)handler;

  struct interrupt_descriptor *entry = &idt[vector];
  entry->address_low = handler_addr & 0xFFFF;
  entry->address_mid = (handler_addr >> 16) & 0xFFFF;
  entry->address_high = handler_addr >> 32;

  entry->selector = 0x08;
  entry->flags = 0b1110 | ((dpl & 0b11) << 5) | (1 << 7);

  entry->ist = 0;
}

void idt_load()
{
  struct idtr idt_reg;
  idt_reg.limit = 0xFFF;
  idt_reg.base = (uint64_t)&idt;
  // asm volatile("lidt %0" ::"m"(&idt_reg));
  __asm__ __volatile__("lidt (%0)" : : "r"(&idt_reg) : "memory");
}

void idt_init()
{
  __asm__ volatile("cli");

  for (int i = 0; i < IDT_SIZE; i++)
  {
    idt[i].flags = 0;
    idt[i].ist = 0;
    idt[i].reserved = 0;
    idt[i].selector = 0;

    idt[i].address_low = 0;
    idt[i].address_mid = 0;
    idt[i].address_high = 0;
  }

  idt_set_entry(0x00, interrupt_service_routine_0, 0);
  idt_set_entry(0x01, interrupt_service_routine_1, 0);
  idt_set_entry(0x02, interrupt_service_routine_2, 0);
  idt_set_entry(0x03, interrupt_service_routine_3, 0);
  idt_set_entry(0x04, interrupt_service_routine_4, 0);
  idt_set_entry(0x05, interrupt_service_routine_5, 0);
  idt_set_entry(0x06, interrupt_service_routine_6, 0);
  idt_set_entry(0x07, interrupt_service_routine_7, 0);
  idt_set_entry(0x08, interrupt_service_routine_err_8, 0);
  idt_set_entry(0x09, interrupt_service_routine_9, 0);

  idt_set_entry(0x0A, interrupt_service_routine_err_10, 0);
  idt_set_entry(0x0B, interrupt_service_routine_err_11, 0);
  idt_set_entry(0x0C, interrupt_service_routine_err_12, 0);
  idt_set_entry(0x0D, interrupt_service_routine_err_13, 0);
  idt_set_entry(0x0E, interrupt_service_routine_err_14, 0);
  idt_set_entry(0x0F, interrupt_service_routine_15, 0);
  idt_set_entry(0x10, interrupt_service_routine_16, 0);
  idt_set_entry(0x11, interrupt_service_routine_err_17, 0);
  idt_set_entry(0x12, interrupt_service_routine_18, 0);

  idt_set_entry(0x20, interrupt_service_routine_32, 0);
  idt_set_entry(0x21, interrupt_service_routine_33, 0);
  idt_set_entry(0x22, interrupt_service_routine_34, 0);

  // remap
  outportb(PIC_MASTER_COMMAND, 0x11);
  outportb(PIC_SLAVE_COMMAND, 0x11);

  outportb(PIC_MASTER_DATA, 0x20);
  outportb(PIC_SLAVE_DATA, 0x28);

  outportb(PIC_MASTER_DATA, 0x04);
  outportb(PIC_SLAVE_DATA, 0x02);

  outportb(PIC_MASTER_DATA, 0x1);
  outportb(PIC_SLAVE_DATA, 0x01);

  outportb(PIC_MASTER_DATA, 0x00);
  outportb(PIC_SLAVE_DATA, 0x00);

  idt_load();

  __asm__ volatile("sti");
}

void print_int(int num)
{
  int a = num;

  if (num < 0)
  {
    putchar('-');
    num = -num;
  }

  if (num > 9)
    print_int(num / 10);

  putchar('0' + (a % 10));
}

struct cpu_status *interrupt_dispatch(struct cpu_status *context)
{

  uint8_t interrupt_number = context->vector_number;

  if (interrupt_number >= 0 && interrupt_number < 0x20)
  {
    puts("vector: ");
    print_int(context->vector_number);
  }

  if (interrupt_number >= 0x20)
  {

    outportb(PIC_MASTER_COMMAND, 0x20); // send EOI (end of interrupt)

    if (0x20 + 8 <= interrupt_number)
    {
      outportb(PIC_SLAVE_COMMAND, 0x20);
    }

    if (interrupt_number == 0x21)
    {
      uint8_t scancode = inportb(0x60);
      // kprintf("%c", scancode);
    }
  }

  return context;
}