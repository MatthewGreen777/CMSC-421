#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "schedulers.h"
#include "task.h"
#include "list.h"
#include "cpu.h"
#include <stdbool.h>

bool FirstPrint = false;

//Create your nodes here
struct node *task_list = NULL;

/**
 * Adds tasks struct to the linked list of tasks
 * @param name The name of the task
 * @param priority The priority of the task
 * @param burst The CPU burst time of the task
 */

void add(char *name, int priority, int burst){

    if (FirstPrint == false) {
        FirstPrint = true;
        printf("-------------------------------------------------------------\n");
        printf("Algorithm Description\n");
	    printf("Priority Round Robin Schedulng: Tasks are executed based on their priority, and tasks with the same priority are executed using round-robin\n");
        printf("-------------------------------------------------------------\n\n");
    }

    //dynamically allocate task and then call insert function 
	printf("Inserting task %s into Linked List\n", name);
	Task *new_task = (Task *)malloc(sizeof(Task));

	new_task->name = strdup(name);
	new_task->priority = priority;
	new_task->burst = burst;

	insert(&task_list, new_task);
}

void schedule() {
    int time_quantum = 10;
    struct node *temp = task_list;
    struct node *highest_priority = temp;

	printf("\nThe Complete Task List (In the order of arrival):\n");

	while (temp != NULL) {
		printf("[%s] [%d] [%d]\n", temp->task->name, temp->task->priority, temp->task->burst);
		temp = temp->next;
	}

	printf("\n");

	temp = task_list;

    while (task_list != NULL) {
        temp = task_list;
        highest_priority = temp;

        while (temp != NULL) {
            if (temp->task->priority > highest_priority->task->priority) {
                highest_priority = temp;
            }
            temp = temp->next;
        }

        temp = task_list;
        while(temp != NULL) {
            if (temp->task->priority == highest_priority->task->priority) {
                int time_slice = (temp->task->burst > time_quantum) ? time_quantum : temp->task->burst;
                run(temp->task, time_slice);

		temp->task->burst -= time_slice;

                if (temp->task->burst == 0) {
                    printf("Task %s has been completed\n", temp->task->name);

                    struct node *prev = temp;
                    temp = temp->next;

		    delete(&task_list, prev->task);
		    break;
                } else {
                    temp = temp->next;
                }
            } else {
                temp = temp->next;
            }
        }
    }
}
