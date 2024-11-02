
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "buffer.h"

struct circular_buffer main_buffer;
bool should_stop = false;

void bubbleSort(int n);

void buffer_init(struct temp_buffer *temp_buffers) {
    // TODO: Initialize the main_buffer and temp_buffers
    // Initialize semaphores and mutexes

    //sem_init 3 parameters: pointer to the semaphore, option if we are sharing the semaphore between processes, and the initial value
    //all main_buffer semaphores will only be shared through threads, so we put 0 for second parameter (idk yet tho)
    if (sem_init(&main_buffer.full, 1, 0) == -1) {
        fprintf(stderr, "ERROR: Error initializing main_buffer.full\n");
        exit(1);
    }

    //do we need to exit after these fail? how to handle these errors idk
    if (sem_init(&main_buffer.empty, 1, 10) == -1) {
        fprintf(stderr, "ERROR: Error initializing main_buffer.empty\n");
        exit(1);
    }

    //semaphore = 1 is open, semaphore = 0 is locked
    if (sem_init(&main_buffer.mutex, 1, 0) == -1) {
        fprintf(stderr, "ERROR: Error initializing main_buffer.mutex\n");
        exit(1);
    }

    //main buffer vars
    main_buffer.count = 0;
    main_buffer.read = 0;
    main_buffer.write = 0;
    main_buffer.producer_exit = 0;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        main_buffer.items[i].app_id = -1;
    }

    //initialize all temp buffers
    for (int i = 0; i < NUM_APPS; i++) {

        if(pthread_mutex_init(&temp_buffers[i].mutex, NULL) != 0){
            fprintf(stderr, "ERROR: Error initializing temp_buffer_%d's mutex\n", i + 1);
            exit(1);
        }

        for (int j = 0; j < TEMP_BUFFER_SIZE; j++) {
            temp_buffers[i].items[j].app_id = -1;
        }

        temp_buffers[i].count = 0;
        temp_buffers[i].consumer_exit = 0;
    }
}

void buffer_cleanup(struct temp_buffer *temp_buffers) {
    // TODO: Destroy semaphores and mutexes

    sem_destroy(&main_buffer.mutex);
    sem_destroy(&main_buffer.empty);
    sem_destroy(&main_buffer.full);
    
    for (int i = 0; i < NUM_APPS; i++) {
        pthread_mutex_destroy(&temp_buffers[i].mutex);
    }
}

void *producer_function(void *arg) {
    printf("Producer function started\n");

    // TODO: Open the input file "input.txt"
    // Read lines and parse them into buffer_item structures
    // Use semaphores to add items to the main_buffer
    // Implement any required processing (e.g., bubbleSort)
    // Handle synchronization properly

    FILE * input;
    unsigned char c = '\0';
    char  * line;
    int buffer_size;
    int curr_size;
    int line_num = 0;

    FILE * producer_log;

    //IDK WHERE TO PUT LOGS AAA this sucks.
    producer_log = fopen("/var/log/received_msgs/producer_log.txt", "w");

    //open input.txt for reading
    input = fopen("input.txt", "r");

    if (input == NULL) {
        fprintf(stderr, "ERROR: Failed to open input.txt");
        exit(1);
    }

    if(producer_log == NULL){
        fprintf(stderr, "ERROR: Failed to open producer log\n");
        exit(1);
    }

    while (!feof(input)) {

        curr_size = 0;
        buffer_size = 50;
        c = '\0';
        line_num++;
        line = (char *) calloc(sizeof(char), buffer_size);

        while (c != '\n' && !feof(input)) {
            c = fgetc(input);

            if (!feof(input) && c != '\n') {
                curr_size++;

                if (curr_size == buffer_size) {
                    buffer_size += 50;
                    line = (char *) realloc(line, buffer_size);

                    if(line == NULL){
                        fprintf(stderr, "ERROR: Realloc failed\n");
                        exit(1);
                    }
                }

                line[curr_size - 1] = c;
            }
        }

        line[curr_size] = '\0';

        //we actually add things to main_buffer in parse_line
        parse_line(line, curr_size, line_num, producer_log, input);

        free(line);

        line = NULL;
    }

    fclose(input);
    fclose(producer_log);

    return NULL;
}

void *consumer_function(void *arg) {
    struct temp_buffer *temp_buffers = (struct temp_buffer *)arg;
    printf("Consumer function started\n");

    // TODO: Use semaphores to consume items from the main_buffer
    // Dispatch items to the appropriate temp_buffer based on app_id
    // Use mutexes to protect access to temp_buffers
    // Implement proper synchronization and exit conditions

    int read_index;
    int temp_buffer_id;
    int temp_buffer_count;

    int count = 10;

    while(!main_buffer.producer_exit || count != 0){

        sem_wait(&main_buffer.full);
        sem_wait(&main_buffer.mutex);

        read_index = main_buffer.read;

        temp_buffer_id = main_buffer.items[read_index].app_id - 1;

        printf("Consumer: recieved item for app %d\n", temp_buffer_id);

        //lock the mutex of the temp buffer we are writing to
        pthread_mutex_lock(&temp_buffers[temp_buffer_id].mutex);

        temp_buffer_count = temp_buffers[temp_buffer_id].count;

        //copy all attributes of the message in main_buffer to the appropriate temp_buffer location
        temp_buffers[temp_buffer_id].items[temp_buffer_count].app_id = main_buffer.items[read_index].app_id;
        temp_buffers[temp_buffer_id].items[temp_buffer_count].priority = main_buffer.items[read_index].priority;
        temp_buffers[temp_buffer_id].items[temp_buffer_count].sleep_interval = main_buffer.items[read_index].sleep_interval;
        temp_buffers[temp_buffer_id].items[temp_buffer_count].timestamp = main_buffer.items[read_index].timestamp;
        strcpy(temp_buffers[temp_buffer_id].items[temp_buffer_count].data, main_buffer.items[read_index].data);

        temp_buffers[temp_buffer_id].count += 1;

        printf("Consumer: moved message to temp buffer %d [App: %d, Priority: %d, Count: %d]\n", temp_buffer_id, temp_buffer_id, main_buffer.items[read_index].priority, temp_buffers[temp_buffer_id].count);

        pthread_mutex_unlock(&temp_buffers[temp_buffer_id].mutex);

        main_buffer.count -= 1;

        main_buffer.read += 1;

        if (main_buffer.read == BUFFER_SIZE) {
            main_buffer.read = 0;
        }

        sem_getvalue(&main_buffer.full, &count);

        sem_post(&main_buffer.mutex);
        sem_post(&main_buffer.empty);

    }

    //signal that consumer is done!!!        
    for (int i = 0; i < NUM_APPS; i++) {
        temp_buffers[i].consumer_exit = 1;
    }

    return NULL;
}

//this function handles writing to the main_buffer
void parse_line(char * line, int line_size, int line_num, FILE * producer_log, FILE * input){

    unsigned char c;
    int num_ints_found = 0;
    int write_index;
    int j;
    int sleep_interval, app_id, priority;

    sem_wait(&main_buffer.empty);
    sem_wait(&main_buffer.mutex);

    write_index = main_buffer.write;

    for (int i = 0; i < line_size; i++) {
        c = line[i];

        //if c is a number
        if (isdigit(c)) { 
            //iterate through the future of the line to find the index of the next space
            for (j = i; !isspace(c); j++) {
                c = line[j + 1];
            }

            //turn that space into a null character temporarily
            line[j] = '\0';

            //we use atoi to turn the string into an int
            if (num_ints_found == 0) {
                main_buffer.items[write_index].app_id = atoi(line + i);
            } else if (num_ints_found == 1) {
                main_buffer.items[write_index].priority = atoi(line + i);
            } else {
                main_buffer.items[write_index].sleep_interval = atoi(line + i);
            }
            
            //turn the null character back into a space
            line[j] = ' ';

            //jump to the space
            i = j;

            num_ints_found++;

        } else if(isalpha(c) && num_ints_found == 3){
            strcpy(main_buffer.items[write_index].data, line + i);
            i = line_size;
        }
    }

    main_buffer.items[write_index].timestamp = time(NULL);

    main_buffer.count += 1;

    sleep_interval = main_buffer.items[write_index].sleep_interval;
    app_id = main_buffer.items[write_index].app_id;
    priority = main_buffer.items[write_index].priority;

    fprintf(producer_log, " Read App id: %d, priority: %d, sleep_interval: %d, message: %s, timestamp: %s", app_id, priority, sleep_interval, main_buffer.items[write_index].data, ctime(&main_buffer.items[write_index].timestamp));

    printf("Processing line %d: app%d %d %d %s\n", line_num, app_id, priority, sleep_interval, main_buffer.items[write_index].data);
    bubbleSort(sleep_interval);

    main_buffer.write += 1;

    //if we wrote to the end of the main buffer, the next index to write to is 0
    if (main_buffer.write == BUFFER_SIZE) {
        main_buffer.write = 0;
    }

    if(feof(input)){
        main_buffer.producer_exit = 1;
    }

    sem_post(&main_buffer.mutex);
    sem_post(&main_buffer.full);

}

void bubbleSort(int n) {
    int arr[n];
    int temp;

    //set random seed
    srand(time(NULL));

    for (int i = 0; i < n; i++) {

        //initialize all members of the array with values between 1 and 100
        arr[i] = rand() % 100;
    }

    // Implement a bubble sort to create random time intervals
    for (int i = 0; i < n - 1; i++) {
    	for (int j = 0; j < n - i - 1; j++) {
    		if (arr[j] > arr[j+1] ) {
    			temp = arr[j];
    			arr[j] = arr[j + 1];
    			arr[j + 1] = temp;
    		}
    	}
    }
}

