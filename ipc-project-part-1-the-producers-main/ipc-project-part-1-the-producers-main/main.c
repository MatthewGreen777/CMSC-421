
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include "buffer.h"
#include "app.h"

struct temp_buffer *shared_temp_buffers;

void create_application_process(int app_id) {
    // TODO: Implement process creation using fork()
    // The child process should call run_application(app_id, shared_temp_buffers)

    pid_t app_pid; 

    char app_id_argument[2];
    //adding '0' to convert the int into char (assuming the int isnt negative)
    char c = app_id + '0';

    app_id_argument[0] = c;
    app_id_argument[1] = '\0';

    app_pid = fork();

    if (app_pid < 0) {
        fprintf(stderr, "ERROR: Forking app %d failed\n", app_id);
    }
    else if (app_pid == 0) {

        //we have forked!
        execl("app", "app", app_id_argument, NULL);

        exit(0);
    }

    printf("Parent created child process %d with PID %d\n", app_id, app_pid);

    //we don't wait here, we wait down in main somewhere
}

int main() {
    bool run = true;
    
    while (run) {
        printf("Main program started\n");
        
        // Create shared memory for temp buffers
        int shm_fd = shm_open("/temp_buffers", O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("shm_open");
            exit(1);
        }

        if (ftruncate(shm_fd, NUM_APPS * sizeof(struct temp_buffer)) == -1) {
            perror("ftruncate");
            exit(1);
        }

        shared_temp_buffers = mmap(0, NUM_APPS * sizeof(struct temp_buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        if (shared_temp_buffers == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        buffer_init(shared_temp_buffers);
        printf("Buffer initialized\n");

    //create shared log
    FILE *shared_log;
    shared_log = fopen("/var/log/IPCproject/all_apps_log.txt", "w");
    
        // Create application processes
        for (int i = 1; i <= NUM_APPS; i++) {
            create_application_process(i);
        }

    pthread_t producer, consumer;
    printf("Creating producer thread\n");
    if (pthread_create(&producer, NULL, producer_function, shared_temp_buffers) != 0) {
        perror("Failed to create producer thread");
        exit(1);
    }
    
    printf("Creating consumer thread\n");
    if (pthread_create(&consumer, NULL, consumer_function, shared_temp_buffers) != 0) {
        perror("Failed to create consumer thread");
        exit(1);
    }

        sem_post(&main_buffer.mutex);

        printf("Waiting for producer thread to finish\n");
        pthread_join(producer, NULL);
        
        printf("Waiting for consumer thread to finish\n");
        pthread_join(consumer, NULL);

        // Wait for all child processes to finish
        for (int i = 0; i < NUM_APPS; i++) {
            int status;
            wait(&status);
            printf("Child process %d exited with status %d\n", i+1, WEXITSTATUS(status));
        }

        printf("Cleaning up buffer\n");

        buffer_cleanup(shared_temp_buffers);

        // Unlink and close shared memory
        if (shm_unlink("/temp_buffers") == -1) {
            perror("Error unlinking shared memory");
        }
        if (close(shm_fd) == -1) {
            perror("Error closing shared memory file descriptor");
        }

        if (fclose(shared_log) != 0) {
            perror("Error closing log file");
        }


        // Check if user wants to run program again
        char input[10];  // Increase size for safety
        do {
            printf("Do you want to run this program again? Yes or No\n");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                // Handle error in fgets
                fprintf(stderr, "Error reading input\n");
                return 1; // Exit if fgets fails
            }

            // Strip newline character if it exists
            input[strcspn(input, "\n")] = 0; // Replace newline with null terminator

            // Convert input to lowercase
            for (int i = 0; input[i]; i++) {
                input[i] = tolower(input[i]);
            }

            // Truncate input to the first 3 characters
            input[3] = '\0'; // Ensure we only consider the first three characters
        } while (strcmp(input, "yes") != 0 && strcmp(input, "no") != 0);


        run = strcmp(input, "yes") == 0; // Corrected condition

        sem_wait(&main_buffer.mutex);
    }
    return 0;
}

