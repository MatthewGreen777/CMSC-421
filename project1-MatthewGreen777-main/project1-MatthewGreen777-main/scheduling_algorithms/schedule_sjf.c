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
	    printf("Shortest Job First (SJF): Tasks are executed from shortest to longest in terms of execution time\n");
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
    struct node *shortest = temp;

	printf("\nThe Complete Task List (In the order of arrival):\n");

	while (temp != NULL) {
		printf("[%s] [%d] [%d]\n", temp->task->name, temp->task->priority, temp->task->burst);
		temp = temp->next;
	}

	printf("\n");

	temp = task_list;
    
    while (task_list != NULL) {
        temp = task_list;
        shortest = temp;

        // Find task with shortest burst
        while (temp != NULL) {
            if (temp->task->burst < shortest->task->burst) {
                shortest = temp;
            }
            temp = temp->next;
        }
        
        printf("Task %s has shortest burst time, so it will run till completion.\n", shortest->task->name);
        run(shortest->task, shortest->task->burst);

        // Free memory for task and node
        delete(&task_list, shortest->task);
    }
}
