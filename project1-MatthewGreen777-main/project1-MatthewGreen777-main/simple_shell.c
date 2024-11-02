/*
project: 01
author: Matthew Green
email: mgreen11@umbc.edu
student id: SN93970
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024  // Buffer size for reading files

// Function declarations
void user_prompt_loop();
char* get_user_command();
char** parse_command(const char *input, int *token_count);
void execute_command(char** args);
void read_proc_file(const char* proc_file);
char *parse_token(const char *token);
char *read_input();

// Main function
int main(int argc, char **argv) {
    if (argc > 1) {
        fprintf(stderr, "Error: This shell does not accept any command-line arguments.\n");
        exit(1);
    }

    // Start the shell loop
    printf("[Simple Shell]\n");
    user_prompt_loop();
    
    return 0;
}

// Function to handle user input and command execution
void user_prompt_loop() {
    char *input;
    char **args;

    while (1) {
        printf(">> ");
        input = read_input();  // Get command from user

        int token_count;
        args = parse_command(input, &token_count);

        if (args[0] == NULL) {
            free(input);
            free(args);
            continue;
        }

        // Handle built-in "exit" command
        if (strcmp(args[0], "exit") == 0) {
            free(input);
            for (int i = 0; i < token_count; i++) {
                free(args[i]);  // Free each parsed token
            }
            free(args);  // Free the array of token pointers
            printf("Thank you for using Simple Shell\n");
            exit(0);  // Exit with status 0
        }

        // Handle "/proc" commands
        if (strncmp(args[0], "/proc", 5) == 0) {
            char proc_file_path[BUFFER_SIZE];

            // Check if there's a space between "/proc" and the filename (args[1] exists)
            if (args[1] != NULL) {
                snprintf(proc_file_path, sizeof(proc_file_path), "%s/%s", args[0], args[1]); // concatenate the path
            } else {
                strncpy(proc_file_path, args[0], sizeof(proc_file_path)); // no space, use the command as is
            }

            // Now call the function to read the /proc file
            read_proc_file(proc_file_path);

        } else {
            // Execute other commands
            execute_command(args);
        }

        free(input);  // Free the original input string
        for (int i = 0; i < token_count; i++) {
            free(args[i]);  // Free each parsed token
        }
        free(args);  // Free the array of token pointers
    }
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
Example: 
    user input: "ls -la"
    parsed output: ["ls", "-la", NULL]
*/
char** parse_command(const char *input, int *token_count) {
    char *input_copy = strdup(input);  // Create a copy of the input
    if (input_copy == NULL) {
        fprintf(stderr, "Memory allocation failed for input copy\n");
        exit(1);
    }

    size_t tokens_size = 10;  // Initial size of the tokens array
    *token_count = 0;
    char **tokens = (char **)malloc(tokens_size * sizeof(char *));
    if (tokens == NULL) {
        fprintf(stderr, "Memory allocation failed for tokens array\n");
        exit(1);
    }

    // Tokenize the input string using spaces as delimiters
    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        // Parse each token (remove quotes and handle escape characters)
        tokens[*token_count] = parse_token(token);
        (*token_count)++;

        // Reallocate memory for tokens array if necessary
        if (*token_count >= tokens_size) {
            tokens_size *= 2;
            tokens = (char **)realloc(tokens, tokens_size * sizeof(char *));
            if (tokens == NULL) {
                fprintf(stderr, "Memory reallocation failed for tokens array\n");
                exit(1);
            }
        }

        // Get the next token
        token = strtok(NULL, " ");
    }

    free(input_copy);  // Free the input copy used for tokenization
    return tokens;
}

/*
execute_command():
Fork a process and execute the parsed command inside the child process
*/
void execute_command(char** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();  // Create a child process

    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Forking error
        perror("fork");
    } else {
        // Parent process waits for the child to finish
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

/*
read_proc_file():
Reads and displays the contents of the specified /proc file.
*/
void read_proc_file(const char* proc_file) {
    char buffer[BUFFER_SIZE];
    int fd, bytes_read;

    // Open the /proc file
    fd = open(proc_file, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error: Could not open %s: %s\n", proc_file, strerror(errno));
        return;
    }

    // Read and print the contents of the file
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the buffer
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        fprintf(stderr, "Error: Could not read %s: %s\n", proc_file, strerror(errno));
    }

    close(fd);
}

// Function to read input dynamically
char *read_input() {
    size_t buffer_size = 256;  // Initial buffer size
    size_t input_len = 0;
    char *input = (char *)malloc(buffer_size * sizeof(char));
    if (input == NULL) {
        fprintf(stderr, "Memory allocation failed for input\n");
        exit(1);
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Resize the buffer if necessary
        if (input_len >= buffer_size - 1) {
            buffer_size *= 2;
            input = (char *)realloc(input, buffer_size * sizeof(char));
            if (input == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                exit(1);
            }
        }
        input[input_len++] = (char)c;
    }

    // Null terminate the input string
    input[input_len] = '\0';

    return input;
}

// Function to handle escape characters and remove quotes for each token
char *parse_token(const char *token) {
    size_t token_len = strlen(token);
    size_t output_len = token_len * 2;  // Estimate output size (handling escape characters)

    // Allocate memory for the output string
    char *output = (char *)malloc(output_len * sizeof(char));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed for output\n");
        exit(1);
    }

    int i = 0, j = 0;

    while (token[i] != '\0') {
        if (token[i] == '"') {
            // Skip quotation marks
            i++;
        } else if (token[i] == '\\') {
            // Handle escape characters
            i++;
            switch (token[i]) {
                case 'n':
                    output[j++] = '\n';
                    break;
                case 't':
                    output[j++] = '\t';
                    break;
                case '\\':
                    output[j++] = '\\';
                    break;
                default:
                    output[j++] = token[i];  // Unknown escape sequence, just add the character
                    break;
            }
            i++;
        } else {
            // Copy regular characters
            output[j++] = token[i++];
        }

        // Reallocate memory if necessary
        if (j >= output_len - 1) {
            output_len *= 2;
            output = (char *)realloc(output, output_len * sizeof(char));
            if (output == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                exit(1);
            }
        }
    }

    // Null terminate the output string
    output[j] = '\0';

    return output;
}
