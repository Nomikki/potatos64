#ifndef SCHELUDING_SCHELUDER_H_
#define SCHELUDING_SCHELUDER_H_

#include <types.h>
#include <arch/x86_64/idt/idt.h>

typedef enum
{
  READY,
  RUNNING,
  DEAD
} status_t;

typedef struct process_t
{
  status_t process_status;
  cpu_status *context;
  struct process_t *next;
} process_t;

extern void init_scheluder();
extern cpu_status *schelude(cpu_status *context);
extern void idle_process();

#endif // SCHELUDING_SCHELUDER_H_