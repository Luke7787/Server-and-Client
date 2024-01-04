#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>    
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>      
#include <errno.h>    
#include <sys/wait.h>
#include <signal.h>  

#define PORT 17794

void handle_sigchld(int sig) {
    /* Wait for all dead processes.
    We use a non-blocking call to avoid hanging if a child hasn't exited yet. */
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_request(int nfd) {
    FILE *network = fdopen(nfd, "r+");
    char *line = NULL;
    size_t size = 0;
    ssize_t num;

    if (network == NULL) {
        perror("fdopen failed");
        close(nfd);
        return;
    }

    printf("Client connected, waiting for request...\n");
    num = getline(&line, &size, network);
    if (num <= 0) {
        perror("getline failed");
        free(line);
        fclose(network);
        return;
    }

    printf("Request received: %s\n", line);
    char method[10], uri[1024], protocol[10];
    if (sscanf(line, "%s %s %s", method, uri, protocol) < 3) {
        fprintf(network, "HTTP/1.1 400 Bad Request\r\n\r\n");
        free(line);
        fclose(network);
        return;
    }

    printf("Method: %s, URI: %s, Protocol: %s\n", method, uri, protocol);
    if (strcmp(method, "GET") != 0) {
        fprintf(network, "HTTP/1.1 501 Not Implemented\r\n\r\n");
        free(line);
        fclose(network);
        return;
    }

    char *filename = uri + 1;
    printf("Attempting to open file: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File open error");
        fprintf(network, "HTTP/1.1 404 Not Found\r\n\r\n");
        free(line);
        fclose(network);
        return;
    }

    // Calculate file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Prepare and send headers
    char headers[1024];
    sprintf(headers, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", fileSize);

    fputs(headers, network);
    fflush(network); // Flush after sending headers

    // Sending file contents
    char fileBuffer[1024];
    while (fgets(fileBuffer, sizeof(fileBuffer), file) != NULL) {
        fputs(fileBuffer, network);
        printf("Sending chunk: %s", fileBuffer); // Debug print
    }

    fflush(network); // Ensure all data is sent
    printf("Finished sending file content.\n");

    // Wait a bit before closing the connection
    sleep(5); // Increased delay to ensure client has time to read

    // Clean up
    fclose(file);
    free(line);
    fclose(network);
}

void run_service(int fd) {
    while (1) {
        int nfd = accept_connection(fd);
        if (nfd != -1) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                close(nfd);
            } else if (pid == 0) { // Child process
                handle_request(nfd);
                exit(0); // Child process exits after handling request
            } else { // Parent process
                close(nfd); // Parent doesn't need this
            }
        }
    }
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart functions if interrupted by handler
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    int fd = create_service(PORT);
    if (fd == -1) {
        perror("Failed to create service");
        exit(1);
    }

    printf("listening on port: %d\n", PORT);
    run_service(fd);
    close(fd);

    return 0;
}
