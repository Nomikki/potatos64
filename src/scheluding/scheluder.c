/*
  TODO:
  allocate memory for processes_list

 */
#include <scheluding/scheluder.h>
#include "config.h"
#include <stdio.h>
#include <mem/kheap.h>

#define PROCESS_MAX 32
process_t **snapshots = NULL;

int currentTask = 0;

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
    printf("A");
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
    printf("B");
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
    printf("C");
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
  return (uint64_t)kmalloc(1024 * 16);
}

process_t *create_process(char *name, void (*_entry_point)(void *), void *arg)
{
  asm("cli");
  process_t *new_process = kmalloc(sizeof(process_t));

  new_process->context = kmalloc(sizeof(cpu_status));
  cpu_status *cpu = new_process->context;
  cpu->vector_number = 0;
  cpu->error_code = 0;
  cpu->iret_rip = (uint64_t)_entry_point;
  cpu->rdi = (uint64_t)arg;
  cpu->iret_cs = 0x8;
  cpu->iret_ss = 0x10;
  cpu->iret_rflags = 0x202;

  void *stack_ponter = alloc_stack();
  if (stack_ponter == NULL)
  {
    printf("Panic!!! rsp is null.\n");
    while (1)
    {
    }
  }

  cpu->iret_rsp = (uint64_t)stack_ponter;
  cpu->rbp = 0;

  printf("cpu ptr: %p\n", cpu);
  printf("entry ip: %p\n", cpu->iret_rip);
  printf("iret_rsp: %p\n", cpu->iret_rsp);

  strcpy(new_process->name, name);

  numTasks++;
  printf("Process created.\n\n");

  asm("sti");
  return new_process;
}

void print_cpu_status(cpu_status *cur_status)
{
  printf("ptr:    %p  [%p]\n", cur_status, (uint64_t)(cur_status) & ~0xFFF);
  printf("rip:    %p  [%p]\n", cur_status->iret_rip, cur_status->iret_rip & ~0xFFF);
  printf("cs:     %p  [%p]\n", cur_status->iret_cs, cur_status->iret_cs & ~0xFFF);
  printf("rflags: %p  [%p]\n", cur_status->iret_rflags, cur_status->iret_rflags & ~0xFFF);
  printf("rsp:    %p  [%p]\n", cur_status->iret_rsp, cur_status->iret_rsp & ~0xFFF);
  printf("rbp:    %p  [%p]\n", cur_status->rbp, cur_status->rbp & ~0xFFF);
  printf("\n");
}

void print_task(int i)
{

  printf("proc [%i / %i]: %i: %s\n", i, numTasks, snapshots[i]->process_status, snapshots[i]->name);
  print_cpu_status(snapshots[i]->context);
}

void print_tasks()
{
  //
  for (int i = 0; i < numTasks; i++)
  {
    if (i == currentTask)
      printf("> ");
    else
      printf("  ");
    print_task(i);
  }
}

void init_scheluder()
{
  printf("Init scheluder\n");
  idt_deactivate();
#ifdef DEBUG
  kheap_print_travel();
#endif

  snapshots = (process_t **)kmalloc(sizeof(process_t *) * PROCESS_MAX);

  for (int i = 0; i < PROCESS_MAX; i++)
  {
    snapshots[i]->context = NULL;
    strcpy(snapshots[i]->name, ":(");
    snapshots[i]->process_status = DEAD;
  }
  numTasks = 0;
  snapshots[numTasks] = create_process("idle", &idle_process, NULL);
  snapshots[numTasks] = create_process("processA", &processA, NULL);
  snapshots[numTasks] = create_process("processB", &processB, NULL);
  snapshots[numTasks] = create_process("processC", &processC, NULL);

  currentTask = 0;

#ifdef DEBUG
  kheap_print_travel();
  print_tasks();
#endif

  printf("Scheluder init ok\n");
  currentTask = 0;
}

cpu_status *schelude(cpu_status *cur_status)
{

  snapshots[currentTask]->context = cur_status;
  snapshots[currentTask]->process_status = READY;

  currentTask++;
  currentTask %= (numTasks);

  snapshots[currentTask]->process_status = RUNNING;

  print_task(currentTask);

  asm("sti");
  return snapshots[currentTask]->context;
}