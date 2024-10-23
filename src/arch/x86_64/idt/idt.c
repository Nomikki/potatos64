#include <arch/x86_64/idt/idt.h>
#include <stdio.h>
#include <string.h>
#include <drivers/serial.h>
#include <scheluding/scheluder.h>
#include <drivers/keyboard.h>
#include <drivers/framebuffer/framebuffer.h>

interrupt_descriptor idt[IDT_SIZE];

void set_idt_entry(uint8_t vector, void *handler, uint8_t dpl)
{
  uint64_t handler_addr = (uint64_t)handler;

  interrupt_descriptor *entry = &idt[vector];
  entry->address_low = handler_addr & 0xFFFF;
  entry->address_mid = (handler_addr >> 16) & 0xFFFF;
  entry->address_high = handler_addr >> 32;

  entry->selector = CODE_SELECTOR;
  entry->flags = PRESENT_BIT | DPL(dpl) | INTERRUPT_GATE;

  entry->ist = 0;
}

void load_idt()
{
  idtr idt_reg;
  idt_reg.limit = 0xFFF;
  idt_reg.base = (uint64_t)&idt;
  // asm volatile("lidt %0" ::"m"(&idt_reg));
  __asm__ __volatile__("lidt (%0)" : : "r"(&idt_reg) : "memory");
}

void init_idt()
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

  set_idt_entry(0x00, interrupt_service_routine_0, 0);
  set_idt_entry(0x01, interrupt_service_routine_1, 0);
  set_idt_entry(0x02, interrupt_service_routine_2, 0);
  set_idt_entry(0x03, interrupt_service_routine_3, 0);
  set_idt_entry(0x04, interrupt_service_routine_4, 0);
  set_idt_entry(0x05, interrupt_service_routine_5, 0);
  set_idt_entry(0x06, interrupt_service_routine_6, 0);
  set_idt_entry(0x07, interrupt_service_routine_7, 0);
  set_idt_entry(0x08, interrupt_service_routine_err_8, 0);
  set_idt_entry(0x09, interrupt_service_routine_9, 0);

  set_idt_entry(0x0A, interrupt_service_routine_err_10, 0);
  set_idt_entry(0x0B, interrupt_service_routine_err_11, 0);
  set_idt_entry(0x0C, interrupt_service_routine_err_12, 0);
  set_idt_entry(0x0D, interrupt_service_routine_err_13, 0);
  set_idt_entry(0x0E, interrupt_service_routine_err_14, 0);
  set_idt_entry(0x0F, interrupt_service_routine_15, 0);
  set_idt_entry(0x10, interrupt_service_routine_16, 0);
  set_idt_entry(0x11, interrupt_service_routine_err_17, 0);
  set_idt_entry(0x12, interrupt_service_routine_18, 0);

  set_idt_entry(0x20, interrupt_service_routine_32, 0); // timer
  set_idt_entry(0x21, interrupt_service_routine_33, 0); // keyboard
  set_idt_entry(0x22, interrupt_service_routine_34, 0);

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

  load_idt();
  //__asm__ volatile("sti");
}

void idt_activate()
{
  asm("sti");
}

void idt_deactivate()
{
  asm("cli");
}

uint64_t read_cr2()
{
  uint64_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

void printStackTrace(size_t level)
{

  StackFrame *cur_frame = __builtin_frame_address(0);
  size_t cur_level = 0;
  printf("Stacktrace:\n");
  while (cur_level < level && cur_frame != NULL)
  {
    printf("[%i] at %p (%p)\n", cur_level, cur_frame->rip, cur_frame->rip & ~0xFFF);
    cur_frame = cur_frame->next;
    cur_level++;
  }
}

void print_cpu_info(cpu_status *context)
{
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

  printStackTrace(10);

  print_pages();
}

cpu_status *interrupt_dispatch(cpu_status *context)
{

  uint8_t interrupt_number = context->vector_number;

  if (interrupt_number >= 0 && interrupt_number < 0x20)
  {
    printf("vector: %x (%i)\n", context->vector_number, context->vector_number);

    switch (interrupt_number)
    {
    case 8:
      printf("\nDouble fault!\n");
      break;

    case 13:
      printf("\nGeneral protection fault!\n");

      printf("Segment error (privilege, type, limit, read/write rights).\n");
      printf("Executing a privileged instruction while CPL != 0.\n");
      printf("Writing a 1 in a reserved register field or writing invalid value combinations (e.g. CR0 with PE=0 and PG=1).\n");
      printf("Referencing or accessing a null-descriptor.\n");
      printf("Accessing a memory address with bits 48-63 not matching bit 47 (e.g. 0x_0000_8000_0000_0000 instead of 0x_ffff_8000_0000_0000) \n");
      printf("Executing an instruction that requires memory operands to be aligned (e.g. movaps) without the proper alignment.\n");
      printf("The saved instruction pointer points to the instruction which caused the exception.\n");
      printf("\n");

      print_cpu_info(context);

      break;

    case 14:
      printf("\nPage fault!\n\n");

      uint64_t faulting_address = read_cr2();

      printf("Error code: %b\n", context->error_code);

      switch (context->error_code)
      {
      case 0:
        printf("Supervisory process tried to read a non-present page entry\n");
        break;
      case ERR_P:
        printf("Supervisory process tried to read a page and caused a protection fault\n");
        break;
      case ERR_RW:
        printf("Supervisory process tried to write to a non-present page entry\n");
        break;
      case ERR_RW | ERR_P:
        printf("Supervisory process tried to write a page and caused a protection fault\n");
        break;
      case ERR_US:
        printf("User process tried to read a non-present page entry\n");
        break;
      case ERR_US | ERR_P:
        printf("User process tried to read a page and caused a protection fault\n");
        break;
      case ERR_US | ERR_RW:
        printf("User process tried to write to a non-present page entry\n");
        break;
      case ERR_US | ERR_RW | ERR_P:
        printf("User process tried to write a page and caused a protection fault\n");

        break;
      default:
        break;
      }

      printf("\nPage fault at address: %p\n\n", faulting_address);
      print_cpu_info(context);

      break;

    default:
      printf("Unhandled interrupt: %i (%x)!\n", interrupt_number, interrupt_number);
      break;
    }

    idt_deactivate();
    while (1)
    {
      clear_framebuffer(128, 27, 26);
      draw_vga_buffer(vga_get_buffer(), 100, 37);
      flip_framebuffer();

      __asm__("hlt");
    }
  }

  if (interrupt_number >= 0x20)
  {

    if (interrupt_number == KEYBOARD_INTERRUPT)
    {
      keyboard_driver_irq_handler();
    }

    if (0x20 + 8 <= interrupt_number)
    {
      outportb(PIC_SLAVE_COMMAND, PIC_EOI);
    }

    if (interrupt_number == TIMER_INTERRUPT)
    {
      idt_deactivate();
      context = schelude(context);
      idt_activate();
    }

    outportb(PIC_MASTER_COMMAND, PIC_EOI); // send EOI (end of interrupt)
  }

  return context;
}