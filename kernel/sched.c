#include <kernel/task.h>
#include <kernel/cpu.h>
#include <inc/x86.h>

#define ctx_switch(ts) \
  do { env_pop_tf(&((ts)->tf)); } while(0)

/*
* Implement a simple round-robin scheduler (Start with the next one)
*
* 1. You have to remember the task you picked last time.
*
* 2. If the next task is in TASK_RUNNABLE state, choose
*    it.
*
* 3. After your choice set cur_task to the picked task
*    and set its state, remind_ticks, and change page
*    directory to its pgdir.
*
* 4. CONTEXT SWITCH, leverage the macro ctx_switch(ts)
*    Please make sure you understand the mechanism.
*/

//
// Lab6
// Modify your Round-robin scheduler to fit the multi-core
// You should:
//
// 1. Design your Runqueue structure first (in kernel/task.c)
//
// 2. modify sys_fork() in kernel/task.c ( we dispatch a task
//    to cpu runqueue only when we call fork system call )
//
// 3. modify sys_kill() in kernel/task.c ( we remove a task
//    from cpu runqueue when we call kill_self system call
//
// 4. modify your scheduler so that each cpu will do scheduling
//    with its runqueue
//    
//    (cpu can only schedule tasks which in its runqueue!!) 
//    (do not schedule idle task if there are still another process can run)	
//
void sched_yield(void)
{
    struct Runqueue *rq = &thiscpu->cpu_rq;
    spin_lock(&rq->lock);

    int cur_idx = rq->cur_task;
    int next_idx = cur_idx;
	do {
	    next_idx = (next_idx + 1) % rq->task_num;
    } while ((next_idx == 0 || rq->tasks[next_idx]->state != TASK_RUNNABLE) &&
             next_idx != cur_idx);

    spin_unlock(&rq->lock);

    // no tasks are runnable, choose idle task
    if (next_idx == cur_idx) next_idx = 0;

    rq->cur_task = next_idx;
    Task *cur_task = thiscpu->cpu_task = rq->tasks[rq->cur_task];
    cur_task->state = TASK_RUNNING;

    lcr3(PADDR(cur_task->pgdir));
    ctx_switch(cur_task);
}
