#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        q->proc[q->size] = proc;
        q->size++;
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        int i;
        int max = 0;
        for (i = 0; i < q->size; i++) {
                if (q->proc[i]->priority > q->proc[max]->priority) {
                        max = i;
                }
        }
        struct pcb_t * proc = q->proc[max];
        for (i = max; i < q->size - 1; i++) {
                q->proc[i] = q->proc[i + 1];
        }
        q->size--;
        return proc;

}

