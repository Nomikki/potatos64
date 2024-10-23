/*
  TODO:
  allocate memory for processes_list

 */
#include <scheluding/scheluder.h>
#include "config.h"
#include <stdio.h>
#include <mem/kheap.h>

process_t processes_list[32];
process_t *current_process = NULL;

// extern uint64_t stack;

void idle_process(void *argv)
{
  while (true)
  {
    asm("hlt");
  }
}

void processA(void *argv)
{
  int a = 0;
  while (true)
  {
    a++;
    if (a > 32000)
    {
      a = 0;
    }
    asm("hlt");
  }
}

void processB(void *argv)
{
  int a = 0;
  while (true)
  {

    a++;
    if (a > 32000)
    {
      a = 0;
    }
    asm("hlt");
  }
}
void processC(void *argv)
{
  int a = 0;
  while (true)
  {

    a++;
    if (a > 32000)
    {
      a = 0;
    }
    asm("hlt");
  }
}

int numTasks = 0;

uint64_t alloc_stack()
{
  return kmalloc(1024 * 2);
}

process_t *process = NULL;

cpu_status *create_process(char *name, void (*_entry_point)(void *), void *arg)
{
  // process_t *new_process = kmalloc(sizeof(process_t));
  asm("cli");
  numTasks++;
  process = (process_t *)kmalloc(sizeof(process_t));
  cpu_status *cpu = (cpu_status *)kmalloc(sizeof(cpu_status));
  process->context = cpu;

  process->context->vector_number = 0;
  process->context->error_code = 0;

  process->context->iret_rip = (uint64_t *)_entry_point;
  process->context->rdi = (uint64_t *)arg;
  process->context->iret_cs = 0x8;
  process->context->iret_ss = 0x10;
  process->context->iret_rflags = 0x202;
  process->context->iret_rsp = alloc_stack();
  process->context->rbp = 0;

  printf("process: %p\n", process);
  printf("cpu: %p\n", cpu);
  printf("entry ip: %p\n", process->context->iret_rip);
  printf("iret_rsp: %p\n", process->context->iret_rsp);

  current_process = process;

  printf("Process created.\n\n");
  asm("sti");
  return process->context;
  // return new_process;
}

int currentTask = 0;
void init_scheluder()
{
  // only 32 processes, hard coded now :[
  for (int i = 0; i < 32; i++)
  {
    processes_list[i].context = NULL;
  }

  current_process = NULL;

  idt_deactivate();
  processes_list[0].context = create_process("idle", idle_process, NULL);
  processes_list[1].context = create_process("processA", processA, NULL);
  processes_list[2].context = create_process("processB", processB, NULL);
  processes_list[3].context = create_process("processC", processC, NULL);
  idt_activate();

  currentTask = 0;
  current_process = &processes_list[currentTask];
}

cpu_status *schelude(cpu_status *cur_status)
{
  // printf("1");
  if (current_process == NULL)
    return cur_status;

  // printf("2");
  current_process->context = cur_status;

  // printf("3");
  currentTask++;
  if (currentTask >= numTasks)
  {
    // printf("!!!");
    currentTask = 0;
  }

  // printf("4");
  current_process = &processes_list[currentTask];

  // printf("5");
  if (/*processes_list[currentTask] == NULL ||*/ processes_list[currentTask].context == NULL)
    return cur_status;

  // printf("6");
  //  return current_process->context;
  return processes_list[currentTask].context;
}