/* Reference: http://www.osdever.net/bkerndev/Docs/pit.htm */
#include <kernel/trap.h>
#include <kernel/picirq.h>
#include <kernel/task.h>
#include <kernel/cpu.h>
#include <inc/mmu.h>
#include <inc/x86.h>

#define TIME_HZ 100

static unsigned long jiffies = 0;

void set_timer(int hz)
{
  int divisor = 1193180 / hz;       /* Calculate our divisor */
  outb(0x43, 0x36);             /* Set our command byte 0x36 */
  outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
  outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* It is timer interrupt handler */
//
// Lab6
// Modify your timer_handler to support Multi processor
// Don't forget to acknowledge the interrupt using lapic_eoi()
//
void timer_handler(struct Trapframe *tf)
{
  extern void sched_yield();
  int i;
    lapic_eoi();
    struct CpuInfo *cpu = thiscpu;

    if (cpu->cpu_id == 0) {
        jiffies++;
    }


    if (cpu->cpu_task != NULL)
    {
    /*
    * 1. Maintain the status of slept tasks
    *
    * 2. Change the state of the task if needed
    *
    * 3. Maintain the time quantum of the current task
    *
    * 4. sched_yield() if the time is up for current task
    *
    */
        struct Runqueue* rq = &cpu->cpu_rq;
        for (int i = 0; i < rq->task_num; ++i) {
            if (rq->tasks[i]->state == TASK_SLEEP) {
                if (--rq->tasks[i]->remind_ticks == 0) {
                    rq->tasks[i]->state = TASK_RUNNABLE;
                    rq->tasks[i]->remind_ticks = TIME_QUANT;
                }
            }
        }

        if (--cpu->cpu_task->remind_ticks == 0) {
            cpu->cpu_task->state = TASK_RUNNABLE;
            cpu->cpu_task->remind_ticks = TIME_QUANT;
            sched_yield();
        }
    }
}

unsigned long sys_get_ticks()
{
  return jiffies;
}
void timer_init()
{
  /*
   * We don't enable timer interrupt here
   * Instead, we use lapic timer
   */

  /* Register trap handler */
  extern void TIM_ISR();
  register_handler( IRQ_OFFSET + IRQ_TIMER, &timer_handler, &TIM_ISR, 0, 0);
}

