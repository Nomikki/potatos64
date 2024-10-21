#ifndef ARCH_X86_64_IDT_IDT_H_
#define ARCH_X86_64_IDT_IDT_H_

#include <types.h>

typedef struct __attribute__((packed))
{
  uint16_t address_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t address_mid;
  uint32_t address_high;
  uint32_t reserved;
} interrupt_descriptor;

typedef struct __attribute__((packed))
{
  uint16_t limit;
  uint64_t base;
} idtr;

typedef struct __attribute__((__packed__))
{
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;

  uint64_t vector_number;
  uint64_t error_code;

  uint64_t iret_rip;
  uint64_t iret_cs;
  uint64_t iret_rflags;
  uint64_t iret_rsp;
  uint64_t iret_ss;
} cpu_status;

typedef struct StackFrame StackFrame;

struct StackFrame
{
  StackFrame *next;
  uint64_t rip;
};

#define IDT_SIZE 256

#define CODE_SELECTOR 0x08
#define INTERRUPT_GATE 0b1110
#define PRESENT_BIT (1 << 7)
#define DPL(dpl) (((dpl) & 0b11) << 5)

#define ERR_P 0b0001
#define ERR_RW 0b0010
#define ERR_US 0b0100

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

/*
  IRQ LINES:
  0: Timer
  1: Keyboard
  2: slave pic
  3: com2
  4: com1
  5: lpt2 or soundcard
  6: floppy disk
  7: lpt1
*/

#define TIMER_INTERRUPT 0x20
#define KEYBOARD_INTERRUPT 0x21

#define PIC_RESET 0x11
#define PIC_MASTER_REMAP_VECTOR 0x20
#define PIC_SLAVE_REMAP_VECTOR 0x28
#define PIC_SLAVE_IRQ_TO_MASTER 0x04
#define PIC_MASTER_IRQ__TO_SLAVE 0x02
#define PIC_8086_MODE 0x01
#define PIC_USE_ALL_INTERRUPTS 0x00

// end of interrupt
#define PIC_EOI 0x20

extern void set_idt_entry(uint8_t vector, void *handler, uint8_t dpl);
extern void load_idt();

extern void interrupt_service_routine_0();
extern void interrupt_service_routine_1();
extern void interrupt_service_routine_2();
extern void interrupt_service_routine_3();
extern void interrupt_service_routine_4();
extern void interrupt_service_routine_5();
extern void interrupt_service_routine_6();
extern void interrupt_service_routine_7();
extern void interrupt_service_routine_err_8();
extern void interrupt_service_routine_9();
extern void interrupt_service_routine_err_10();
extern void interrupt_service_routine_err_11();
extern void interrupt_service_routine_err_12();
extern void interrupt_service_routine_err_13();
extern void interrupt_service_routine_err_14();
extern void interrupt_service_routine_15();
extern void interrupt_service_routine_16();
extern void interrupt_service_routine_err_17();
extern void interrupt_service_routine_18();
extern void interrupt_service_routine_32();
extern void interrupt_service_routine_33();
extern void interrupt_service_routine_34();
extern void interrupt_service_routine_255();

extern cpu_status *interrupt_dispatch(cpu_status *context);

extern void print_cpu_info(cpu_status *context);

extern void init_idt();
extern void idt_activate();
extern void idt_deactivate();

#endif // ARCH_X86_64_IDT_IDT_H_