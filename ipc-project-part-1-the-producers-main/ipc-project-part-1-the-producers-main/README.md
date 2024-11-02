[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/JCrfNn6v)

CMSC 421: InterProcess Communication (IPC) Project

This project implements a multi-threaded C application simulating a producer-consumer model with inter-process communication using shared memory, semaphores, and mutexes. The program consists of a producer that reads messages from a file and dispatches them to multiple consumers through a circular buffer, with each consumer forwarding messages to application processes. These application processes monitor a temporary buffer, process the messages, and log events.

Project Overview

The project consists of several components working in coordination:

- Producer: Reads messages from an input file, adds timestamps, and logs activity. Messages are inserted into a circular buffer shared with consumers.
- Consumer: Retrieves messages from the circular buffer, dispatching each to one of three application processes based on the message's app_id.
- Application Processes: Each application attaches to its specific temporary buffer and monitors it periodically, processing and logging new messages.
- Logging: Logs capture each process's activity, including message transfers and timestamps, stored in the /var/log/IPCproject/ directory.

Project Files

- main.c: Main entry point. Initializes shared resources, creates threads, and forks application processes.
- buffer.h and buffer.c: Circular buffer implementation and function definitions for producer-consumer data handling.
- app.c: Defines application process behavior, attaching to temporary buffers and logging events.
- Makefile: Simplifies compilation and clean-up processes.
- input1.txt: Sample input file for testing message production and consumption.

Requirements

- Platform: Linux environment.
- Compiler: GCC
- Libraries: POSIX threads, semaphores, shared memory

**Compilation**

To compile the project, use the Makefile provided. Simply run:

make

**Usage**

**1\. Running the Program**

After compiling, run the executable:

./main input1.txt

The program will:

1. Initialize shared resources, including semaphores and shared memory for the circular and temporary buffers.
2. Start the producer and consumer threads.
3. Fork three application processes (App1, App2, App3), each assigned a unique temporary buffer.
4. Begin message processing, logging events as they occur.

**2\. Input and Message Format**

The producer reads messages from input1.txt. Messages should follow a simple line-by-line format. Each line represents a message, and fields such as timestamp and app_id are added or parsed during processing.

**3\. Logs**

Logs capture all message transfer and processing events:

- Producer Log: /var/log/IPCproject/producer_log.txt - Logs each message’s timestamp and insertion into the circular buffer.
- Consumer Log: /var/log/IPCproject/consumer_log.txt - Logs each message’s retrieval from the circular buffer.
- Application Logs:
- /var/log/IPCproject/app1_log.txt
- /var/log/IPCproject/app2_log.txt
- /var/log/IPCproject/app3_log.txt
- Combined Application Log: /var/log/IPCproject/all_apps_log.txt

Logs are created automatically if they do not already exist.

**4\. Exiting the Program**

After the producer processes all messages in input1.txt, the program will prompt the user to either reprocess the file or exit. To cleanly exit, ensure all resources are released by responding with the exit option.

Testing and Troubleshooting

1. Message Processing: Verify message timestamps and IDs in logs to confirm accurate message flow between components.
2. Resource Allocation: Check shared memory and semaphore usage. Use tools like top and valgrind to ensure proper memory and process management.
3. Optional Extra Credit: App3 can be configured to prioritize messages based on a priority field.

Notes

- Ensure proper permissions are set for creating logs in /var/log/IPCproject/.
- Each application monitors its temporary buffer every 3 seconds, so expect a delay in logs for low-frequency message activity.
