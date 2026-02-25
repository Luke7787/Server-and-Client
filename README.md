# Webserver Implementation

## Overview

CSC 357 Systems Programming project at Cal Poly where I implemented an HTTP web server in C with concurrent client handling via child processes in a UNIX environment.

## Implementation Details

1. **Extend Echo Server**
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

## Learning Objectives

### Direct
- **Read and write complex C programs.**
- **Discuss the UNIX operating system architecture and use operating system services (system calls) directly.**
- **Differentiate between language features and operating system features.**
- **Gain experience with low-level programming in the UNIX environment.**
- **Consider basic resource management in program development.**

## Running the Project

1. **Clone the Repository**
   - First, clone the repository to your local machine:
     ```bash
     git clone https://github.com/your-repository-url.git
     cd your-repository-directory
     ```

2. **Build the Project**
   - In one terminal, navigate to the project directory and compile the source code:
     ```bash
     make
     ```

3. **Start the Webserver**
   - In a second terminal, start the webserver by running the compiled executable:
     ```bash
     ./webserver
     ```

4. **Test the Server**
   - Use a web browser or a tool like `curl` in a third terminal or another application to send a `GET` request to the server. For example:
     ```bash
     curl http://localhost:port/source.txt
     ```
   - Replace `port` with the port number your server is listening on.
