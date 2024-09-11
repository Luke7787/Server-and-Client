# Simple Webserver Implementation

## Introduction

A project for CSC 357: Systems Programming from California Polytechnic State University. This assignment involves building a simple webserver that supports a subset of the Hypertext Transfer Protocol (HTTP). The webserver will manage multiple client requests using child processes, providing hands-on experience with system-level programming in C and UNIX environments.

## Overview

This assignment involves implementing a simple webserver that supports a subset of the Hypertext Transfer Protocol (HTTP). The webserver will handle multiple client requests by forking child processes for each request.

## Course Learning Objectives

### Direct
- **Read and write complex C programs.**
- **Transition to upper-division coursework where programming is used to explore course-specific content.**
- **Move from following stepwise instructions to independent problem solving.**
- **Discuss the UNIX operating system architecture and use operating system services (system calls) directly.**
- **Differentiate between language features and operating system features.**
- **Gain experience with low-level programming in the UNIX environment.**
- **Consider basic resource management in program development.**

### Indirect
- **Use standard UNIX user-level commands and the UNIX software development environment.**

## Requirements

### Functional Overview

1. **Extend Echo Server**
   - Start with the echo server solution from Lab 7.
   - Modify the echo server to spawn a child process for each client request.
   - The primary (parent) process should register a signal handler for the `SIGCHLD` signal and wait within that handler. 
   - Use `waitpid` with `WNOHANG` and a loop to handle all pending child processes:
     ```c
     while (waitpid(-1, NULL, WNOHANG) > 0);
     ```

2. **Implement HTTP Protocol**
   - Your server should implement a subset of HTTP, specifically handling `GET` requests.
   - Each request will be a single text line in the format: `GET filename`.
   - For example: `GET source.txt`
   - The server should locate the requested file and, if found, send its contents back to the client.
   - Ensure the server does not close the socket until the client does.

3. **Create Test File**
   - For grading purposes, create a file named `source.txt` in the same directory as your client and server files. 
   - The content of `source.txt` can be a short line of text.

## Deliverables

- **Source Code**: Push all relevant source code and Makefile to your repository.

## Grading Rubric

- **Functionality – Handling Clients through `fork()`**: 25 points
- **Functionality – Using Signals to Handle Clients**: 20 points
- **Functionality – HTTP Implementation on Server Side**: 20 points
- **Functionality – HTTP Implementation on Client Side**: 15 points
- **Program Style**: 10 points
  - The source code should follow best practices regarding line length, function decomposition, naming conventions, and documentation.
- **Clean Valgrind Report**: 10 points

**Total**: 100 points
