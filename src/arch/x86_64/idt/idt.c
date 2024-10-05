#include <arch/x86_64/idt/idt.h>
#include <stdio.h>
#include <string.h>

#include <drivers/keyboard.h>
#include <drivers/framebuffer/framebuffer.h>

struct interrupt_descriptor idt[IDT_SIZE];

void idt_set_entry(uint8_t vector, void *handler, uint8_t dpl)
{
  uint64_t handler_addr = (uint64_t)handler;

  struct interrupt_descriptor *entry = &idt[vector];
  entry->address_low = handler_addr & 0xFFFF;
  entry->address_mid = (handler_addr >> 16) & 0xFFFF;
  entry->address_high = handler_addr >> 32;

  entry->selector = CODE_SELECTOR;
  entry->flags = PRESENT_BIT | DPL(dpl) | INTERRUPT_GATE;

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

  idt_set_entry(0x20, interrupt_service_routine_32, 0); // timer
  idt_set_entry(0x21, interrupt_service_routine_33, 0); // keyboard
  idt_set_entry(0x22, interrupt_service_routine_34, 0);

  // remap
  outportb(PIC_MASTER_COMMAND, PIC_RESET);
  outportb(PIC_SLAVE_COMMAND, PIC_RESET);

  outportb(PIC_MASTER_DATA, PIC_MASTER_REMAP_VECTOR);
  outportb(PIC_SLAVE_DATA, PIC_SLAVE_REMAP_VECTOR);

  outportb(PIC_MASTER_DATA, PIC_SLAVE_IRQ_TO_MASTER);
  outportb(PIC_SLAVE_DATA, PIC_MASTER_IRQ__TO_SLAVE);

  outportb(PIC_MASTER_DATA, PIC_8086_MODE);
  outportb(PIC_SLAVE_DATA, PIC_8086_MODE);

  outportb(PIC_MASTER_DATA, PIC_USE_ALL_INTERRUPTS);
  outportb(PIC_SLAVE_DATA, PIC_USE_ALL_INTERRUPTS);

  idt_load();
  __asm__ volatile("sti");
}

uint64_t read_cr2()
{
  uint64_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

struct cpu_status *interrupt_dispatch(struct cpu_status *context)
{

  uint8_t interrupt_number = context->vector_number;

  if (interrupt_number >= 0 && interrupt_number < 0x20)
  {
    // printf("vector: %x (%i)\n", context->vector_number, context->vector_number);

    switch (interrupt_number)
    {
    case 8:
      printf("\nDouble fault!\n");
      break;

    case 13:
      printf("\nGeneral protection fault!\n");
      break;

    case 14:
      printf("\nPage fault!\n");

      uint64_t faulting_address = read_cr2();
      // printf("error code (memory access that triggered the page fault): %x\n", context->error_code);
      printf("Error code: %b\n", context->error_code);
      printf("0:%b = Present\n", context->error_code & 0b1 ? 1 : 0);
      printf("1:%b = Write\n", context->error_code & 0b10 ? 1 : 0);
      printf("2:%b = User\n", context->error_code & 0b100 ? 1 : 0);
      printf("3:%b = Reserved bit set\n", context->error_code & 0b1000 ? 1 : 0);
      printf("4:%b = Instruction fetch\n\n", context->error_code & 0b10000 ? 1 : 0);

      printf("Page fault at address: %p\n\n", faulting_address);

      printf("rax: %p   ", context->rax);
      printf("rbx: %p\n", context->rbx);
      printf("rcx: %p   ", context->rcx);
      printf("rdx: %p\n\n", context->rdx);

      printf("rdi: %p\n", context->rdi);
      printf("rsi: %p   ", context->rsi);
      printf("rbp: %p\n\n", context->rbp);

      printf("r8:  %p   ", context->r8);
      printf("r9:  %p\n", context->r9);
      printf("r10: %p   ", context->r10);
      printf("r11: %p\n", context->r11);
      printf("r12: %p   ", context->r12);
      printf("r13: %p\n", context->r13);
      printf("r14: %p   ", context->r14);
      printf("r15: %p\n\n", context->r15);

      printf("iret cs:     %p   ", context->iret_cs);
      printf("iret rip:    %p\n", context->iret_rip);
      printf("iret rps:    %p   ", context->iret_rsp);
      printf("iret ss:     %p\n\n", context->iret_ss);
      printf("iret rflags: %p\n", context->iret_rflags);
      printf("             %B\n", context->iret_rflags);
      printf("           64|             48|             32|             16|              0\n");
      printf("             FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210\n");

      break;

    default:
      printf("Unhandled interrupt: %i (%x)!\n", interrupt_number, interrupt_number);
      break;
    }

    while (1)
    {
      framebuffer_clear(128, 27, 26);
      videobuffer_draw_vga_buffer(vga_get_buffer(), 100, 37);
      framebuffer_flip();
      __asm__("hlt");
    }
  }

  if (interrupt_number >= 0x20)
  {

    outportb(PIC_MASTER_COMMAND, PIC_EOI); // send EOI (end of interrupt)

    if (0x20 + 8 <= interrupt_number)
    {

      outportb(PIC_SLAVE_COMMAND, PIC_EOI);
    }

    if (interrupt_number == 0x21)
    {
      keyboard_driver_irq_handler();
    }
  }

  return context;
}