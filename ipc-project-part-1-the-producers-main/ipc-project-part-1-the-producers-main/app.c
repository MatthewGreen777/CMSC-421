#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include "app.h"
#include "buffer.h"

static int app_id;
static struct temp_buffer *my_temp_buffer;

struct temp_buffer *shared_temp_buffers;

void *check_temp_buffer(void *arg) {
    printf("App %d: check_temp_buffer started\n", app_id + 1);

    // TODO: Open log files for writing
    // Continuously check my_temp_buffer for new messages
    // Process messages and log the output
    // Implement exit conditions and cleanup

    //adding '0' to convert the int into char (assuming the int isnt negative)
    char app_id_char = (app_id + 1) + '0';

    char personal_log_name[34] = "/var/log/IPCproject/appX_log.txt";
    //23 is the index in the string with the X
    personal_log_name[23] = app_id_char;

    FILE * personal_log;
    FILE * shared_log;

    int temp_app_id;
    int temp_priority;
    char * temp_time;

    personal_log = fopen(personal_log_name, "w");

    shared_log = fopen("/var/log/IPCproject/all_apps_log.txt", "a");

    if (personal_log == NULL) {
        fprintf(stderr, "ERROR: App %d failed to open personal_log\n", app_id + 1);
        exit(1);
    }

    if(shared_log == NULL){
        fprintf(stderr, "ERROR: App %d failed to open shared_log\n", app_id + 1);
        exit(1);
    }

    //while the temp buffer still has stuff in it and the consumer has no more data for it
    while (!((*my_temp_buffer).consumer_exit) || (*my_temp_buffer).count != 0) {

        //stall 3 seconds
        sleep(3);

        //acquire control of the temp buffer associated with the process
        pthread_mutex_lock(&my_temp_buffer->mutex);
        
        //check temp buffer 
        for (int i = 0; i < TEMP_BUFFER_SIZE; i++) {
            if (my_temp_buffer->items[i].app_id >= 0) {
                temp_app_id = my_temp_buffer->items[i].app_id;
                temp_priority = my_temp_buffer->items[i].priority;

                //ctime returns a string with the current local time stamp
                temp_time = ctime(&my_temp_buffer->items[i].timestamp);
                
                fprintf(personal_log, "App %d recieved: Priority %d, Data: %s, Time: %s", temp_app_id, temp_priority, my_temp_buffer->items[i].data, temp_time);
                fprintf(shared_log, "App %d recieved: Priority %d, Data: %s, Time: %s", temp_app_id, temp_priority, my_temp_buffer->items[i].data, temp_time);

                printf("App %d processed message: App ID: %d, Priority %d, Data: %s\n", temp_app_id, temp_app_id, temp_priority, my_temp_buffer->items[i].data);

                //indicate that we have already read the message
                my_temp_buffer->items[i].app_id = -1;

                my_temp_buffer->count -= 1;
            }
        }

        //relinquish control of temp buffer
        pthread_mutex_unlock(&my_temp_buffer->mutex);
    }    

    fclose(personal_log);
    fclose(shared_log);

    return NULL;
}

void run_application(int id, struct temp_buffer *temp_buffers) {
    app_id = id - 1;
    my_temp_buffer = &temp_buffers[app_id];

    printf("Application %d started, temp buffer address: %p\n", app_id + 1, (void*)my_temp_buffer);

    // TODO: Create a thread that runs check_temp_buffer
    // Wait for the thread to finish
    // Perform any necessary cleanup

    pthread_t t1;

    int creation_errno;

    //if attr attribute is NULL, default attributes used
    if ((creation_errno = pthread_create(&t1, NULL, check_temp_buffer, NULL)) != 0) {
        fprintf(stderr, "ERROR: pthread_create returned with error code %d", creation_errno);
    }

    pthread_join(t1, NULL);

    //CLEANUP... NEED TO CODE

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <app_id>\n", argv[0]);
        exit(1);
    }

    int app_id = atoi(argv[1]);
    printf("App started with app_id: %d\n", app_id);

    int shm_fd = shm_open("/temp_buffers", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    shared_temp_buffers = mmap(0, NUM_APPS * sizeof(struct temp_buffer), 
                               PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_temp_buffers == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    run_application(app_id, shared_temp_buffers);

    return 0;
}
