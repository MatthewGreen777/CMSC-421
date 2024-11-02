/**
 * Various list operations
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"


// add a new task to the list of tasks
void insert(struct node **head, Task *newTask) {
    // add the new task to the list 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->task = newTask;
    newNode->next = NULL;

	if (*head == NULL) {
		*head = newNode;
	} else {
		struct node *temp = *head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

// delete the selected task from the list
void delete(struct node **head, Task *task) {
    struct node *temp = *head;
    struct node *prev = NULL;


    // If list is empty
    if (temp == NULL) {
        return;
    }

    // special case - beginning of list
    if (strcmp(temp->task->name, task->name) == 0) {
        *head = temp->next;
        free(temp->task->name);
        free(temp->task);
        free(temp);
        return;
    }

    while (temp != NULL && strcmp(temp->task->name, task->name) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    prev->next = temp->next;

    free(temp->task->name);
    free(temp->task);
    free(temp);
}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }
}
