#include <scheluding/scheluder.h>
#include "config.h"
#include <stdio.h>
#include <mem/kheap.h>

process_t *current_process = NULL;

process_t *processes_list[32];

extern uint64_t stack;

void idle_process(void *argv)
{
  while (true)
  {
    printf(".");
    // printf(".");dwadwa
    asm("hlt");
  }
}

void processA(void *argv)
{
  printf("AA %p\n", processA);
  int k = 0;
  while (1)
  {

    k++;
    if (k == 3200 - 20)
    {
      printf("A");
      asm("hlt");
      k = 0;

      /*
      uint64_t *ptr = kmalloc(10);
      kfree(ptr);
      */
    }
  }
}
void processB(void *argv)
{
  printf("BB %p\n", processB);
  int k = 0;
  while (1)
  {
    k++;
    if (k == 3200)
    {
      printf("B");
      asm("hlt");
      k = 0;
    }
  }
}
void processC(void *argv)
{
  printf("CC %p\n", processC);
  int k = 0;
  while (1)
  {
    k++;
    if (k == 3200)
    {
      printf("C");
      asm("hlt");
      k = 0;
    }
  }
}

int numTasks = 0;
process_t *process = NULL;
process_t *create_process(void (*_entry_point)(void *))
{
  numTasks++;
  process = (process_t *)kmalloc(sizeof(process_t));
  process->context = (cpu_status *)kmalloc(sizeof(cpu_status));
  void *stack_addr = kmalloc(1024 * 2);

  process->context->r15 = 0;
  process->context->r14 = 0;
  process->context->r13 = 0;
  process->context->r12 = 0;
  process->context->r11 = 0;
  process->context->r10 = 0;
  process->context->r9 = 0;
  process->context->r8 = 0;

  process->context->rsi = 0;
  process->context->rbp = 0;
  process->context->rdx = 0;
  process->context->rcx = 0;
  process->context->rbx = 0;
  process->context->rax = 0;

  process->context->vector_number = 0;
  process->context->error_code = 0;

  // process->context->iret_ss = 0;
  // process->context-> cpuState = (CPUstate *)(stack + 4096 - sizeof(CPUstate));

  process->context->iret_rip = (void *)_entry_point;
  process->context->iret_cs = 0x8;
  process->context->iret_ss = 0x10;
  process->context->iret_rflags = 0x202;
  process->context->iret_rsp = stack_addr;
  process->context->rdi = (void *)_entry_point;
  printf("created rps addr: %p\n", stack_addr);
  printf("iret rip %x\n", process->context->iret_rip);

  // 0xFFFFFFFF8018C028

  printf("Process created.\n");
  // printf("addr of stack: %p\n", &stack);
  return process;
}
int currentTask = 0;
void init_scheluder()
{
  processes_list[0] = create_process(idle_process);
  processes_list[1] = create_process(processA);
  processes_list[2] = create_process(processB);
  processes_list[3] = create_process(processC);

  currentTask = 0;
  current_process = processes_list[currentTask];
}

cpu_status *schelude(cpu_status *context)
{
  __asm__("cli");

  current_process->context = context;

  currentTask++;
  if (currentTask >= numTasks)
  {
    currentTask = 0;
  }
  current_process = processes_list[currentTask];

  // printf("task: %i\n", currentTask);
  __asm__("sti");

  return current_process->context;
}