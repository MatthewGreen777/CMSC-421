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
	    printf("First-Come, First-Served (FCPS): Tasks are executed in the order they arrive.\n");
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

// function to free the memory allocated for task 
void free_task(Task *task) {
    if(task) {
        free(task->name);
        free(task);
    }
}

/**
 * Invokes the scheduler to run the task
 * Implement FCFS scheduling algorithm here
 */
void schedule(){

	struct node *temp = task_list;
    struct node *prev;

	printf("\nThe Complete Task List (In the order of arrival):\n");

	while (temp != NULL) {
		printf("[%s] [%d] [%d]\n", temp->task->name, temp->task->priority, temp->task->burst);
		temp = temp->next;
	}

	printf("\n");

	temp = task_list;

	while (temp != NULL) {
		printf("Task %s arrived first, so it will run till completion.\n", temp->task->name);
		run(temp->task, temp->task->burst);

        prev = temp;
		temp = temp->next;

        delete(&task_list, prev->task);
	}
}
