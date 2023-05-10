
#include "queue.h"
#include "sched.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
extern int time_slot;
static int run_slot[MAX_PRIO];
#endif

int queue_empty(void) {
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if(!empty(&mlq_ready_queue[prio])) 
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
#ifdef MLQ_SCHED
    int i ;

	for (i = 0; i < MAX_PRIO; i ++)
		mlq_ready_queue[i].size = 0;
	for(int prio = 0 ; prio < MAX_PRIO; prio++)
	{
		run_slot[prio] = MAX_PRIO - prio;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/* 
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
void queue_time_reset(void)
{
	for(int prio = 0 ; prio < MAX_PRIO; prio++)
	{
		run_slot[prio] = MAX_PRIO - prio;
	}
}
struct pcb_t *get_mlq_proc(void)
{
	struct pcb_t *proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */

	pthread_mutex_lock(&queue_lock);
	
	bool reset = false;
	int i;
	for (i = 0; i < MAX_PRIO; i++)
	{
		if (!empty(&mlq_ready_queue[i]))
		{

			if (run_slot[i] > 0)
			{
			
			
				proc = dequeue(&mlq_ready_queue[i]);
				reset = false;
				break;
			}
			else
			{
				reset = true;
			}
		}
	}
	if(i == MAX_PRIO) 
	{
		//no process is found 
		if(reset)
		{
			//if there still process in queue but no slot left, but hasn't finished
			queue_time_reset();
			return get_mlq_proc();
		}
		pthread_mutex_unlock(&queue_lock);
		return NULL;
	}


	if (run_slot[i] > time_slot)
	
	{
		run_slot[i] -= time_slot;
	}
	else
	{
		run_slot[i] = 0;
	}
	pthread_mutex_unlock(&queue_lock);
	return proc;
}

void put_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(void) {
	return get_mlq_proc();
}

void put_proc(struct pcb_t * proc) {
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t * proc) {
	return add_mlq_proc(proc);
}
#else
struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	return proc;
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}
#endif


