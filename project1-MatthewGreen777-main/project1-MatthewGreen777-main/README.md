[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/7Q840MxU)
# Simple Shell Project
**CMSC 421 - Fall 2024 - Sections 1, 2, and 3**  
**Project 1: Simple Shell, /proc Directory, and CPU Scheduling Algorithms**  
**Due Date:** Sunday, October 13 by 11:59 PM  

## Table of Contents
1. [Introduction](#introduction)
2. [Part 1: Simple Shell](#part-1-simple-shell)
    - [Features](#features)
    - [Usage](#usage)
    - [Example Commands](#example-commands)
    - [Memory Management](#memory-management)
3. [Part 2: /proc Directory Exploration](#part-2-proc-directory-exploration)
    - [Features](#features-1)
    - [Usage](#usage-1)
    - [Example Commands](#example-commands-1)
4. [Part 3: CPU Scheduling Algorithms](#part-3-cpu-scheduling-algorithms)
    - [Algorithms Implemented](#algorithms-implemented)
    - [Compilation and Execution](#compilation-and-execution)
5. [How to Compile and Run](#how-to-compile-and-run)
6. [Assumptions & Special Details](#assumptions--special-details)
7. [Commit History](#commit-history)
8. [Notes](#notes)
9. [References](#references)

---

## Introduction

This project consists of three parts:
1. **Part 1: Basic Shell Implementation** - Develop a simple Unix/Linux shell that handles basic commands, argument parsing, and the `exit` command.
2. **Part 2: `/proc` Directory Exploration** - Extend the shell to retrieve and display information from the Linux `/proc` directory.
3. **Part 3: CPU Scheduling Algorithms** - Implement five different CPU scheduling algorithms as a separate mini-project.

The project is designed to reinforce low-level C programming and introduce key system programming concepts, such as process management and scheduling. This project must be completed in C and must run on the Debian VM provided for the class.

---

## Part 1: Simple Shell

### Features
- **No Arguments**: The shell accepts no arguments and prompts the user for commands.
- **Command Parsing**: The shell parses user input into a command and its arguments, supporting both relative and absolute paths.
- **Multiple Arguments**: The shell can handle multiple arguments passed to a command.
- **Built-in Exit Command**: The shell supports the `exit` command, which can optionally accept an integer argument for the exit status.
- **Dynamic Memory**: The shell dynamically handles input of arbitrary length using `malloc()` and `realloc()`.
- **Error Handling**: The shell returns appropriate error messages for invalid input or execution failures.

### Usage

1. Compile the shell using the provided `Makefile`:
    ```bash
    make
    ```

2. Run the shell:
    ```bash
    ./simple_shell
    ```

3. Execute commands within the shell:
    ```bash
    $ ls
    $ /bin/echo "Hello World"
    $ exit 0
    ```

### Example Commands
- **Absolute Path Execution**: `/usr/bin/ls`
- **Relative Path Execution**: `ls -la`
- **Exit**: `exit 0`

### Memory Management
- The shell uses `malloc()` to dynamically allocate memory and `free()` to prevent memory leaks. The Valgrind tool is used to verify the absence of memory leaks.

---

## Part 2: /proc Directory Exploration

### Features
The shell supports retrieving information from the following files in the `/proc` directory:
- `/proc/cpuinfo`
- `/proc/loadavg`
- `/proc/filesystems`
- `/proc/mounts`
- `/proc/[pid]/status`
- `/proc/[pid]/environ`
- `/proc/[pid]/sched`

### Usage

To retrieve information from the `/proc` directory, use the `proc` command followed by the desired file:
```bash
$ proc cpuinfo
$ proc loadavg
$ proc 1234/status
```

## Part 3: CPU Scheduling Algorithms
Algorithms Implemented
First-Come, First-Served (FCFS)
Shortest-Job-First (SJF)
Priority Scheduling
Round-Robin (RR)
Priority with Round-Robin (Priority RR)
Compilation and Execution
Use the Makefile to compile the scheduling algorithms:

Compile FCFS Scheduler:

```bash
make fcfs
Run the Scheduler:
./fcfs schedule.txt
```
Repeat these steps for each of the other scheduling algorithms by substituting fcfs with the appropriate algorithm name (sjf, priority, rr, priority_rr):

```bash
make sjf
./sjf schedule.txt

make priority
./priority schedule.txt

make rr
./rr schedule.txt

make priority_rr
./priority_rr schedule.txt
```

Clone the repository:

```bash
git clone git@github.com:UMBC-CMSC421/project1-username.git
cd project1-username
Compile the project:
```
```bash
make
Run the shell:
```
```bash
./simple_shell
For CPU scheduling algorithms:
```
```bash
make fcfs
./fcfs schedule.txt
```
## Assumptions & Special Details
The shell only handles simple commands and basic argument parsing. It does not support scripting, file input, or I/O redirection.
Memory management is handled using malloc() and free(), with no memory leaks as confirmed by Valgrind.
The /proc exploration does not involve parsing the content of the files; it simply prints the file contents as they are.
For the CPU scheduling algorithms:
Tasks are assumed to arrive simultaneously.
The scheduler does not need to handle preemptions for non-RR algorithms.
Time quantum for Round-Robin scheduling is 10 milliseconds.
Priorities range from 1 to 10, where a higher number indicates a higher priority.
Commit History
The project was developed incrementally with regular commits:

Initial Commit: Project setup and basic shell framework.
Argument Parsing: Added command parsing and dynamic memory allocation.
/proc Directory Support: Implemented proc command functionality.
CPU Scheduling Algorithms: Completed all five scheduling algorithms with detailed output.
Testing and Debugging: Final cleanup, memory leak checks, and validation using Valgrind.
## Notes
Ensure that the project is compiled and tested in the provided Debian VM, as this is the environment in which grading will be performed.
All memory allocations are properly freed before the shell exits, and all processes are correctly managed.
The scheduling algorithms provide informative statements that explain each step of the algorithm as it executes.

