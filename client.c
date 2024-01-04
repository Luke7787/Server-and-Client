#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PORT 17794

#define MIN_ARGS 2
#define MAX_ARGS 2
#define SERVER_ARG_IDX 1

#define USAGE_STRING "usage: %s <server address>\n"

void validate_arguments(int argc, char *argv[])
{
    if (argc == 0)
    {
        fprintf(stderr, USAGE_STRING, "client");
        exit(EXIT_FAILURE);
    }
    else if (argc < MIN_ARGS || argc > MAX_ARGS)
    {
        fprintf(stderr, USAGE_STRING, argv[0]);
        exit(EXIT_FAILURE);
    }
}

void send_request(int fd) {
    printf("Enter the name of the file you want to GET: ");
    char filename[1024];
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Error reading filename");
        return;
    }

    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    char getRequest[2048];
    snprintf(getRequest, sizeof(getRequest), "GET /%s HTTP/1.1\r\n\r\n", filename);
    printf("Sending GET request: %s\n", getRequest);
    if (write(fd, getRequest, strlen(getRequest)) == -1) {
        perror("write failed");
        return;
    }

    printf("Receiving response...\n");
    char responseHeader[4096] = {0};
    ssize_t bytesRead;
    ssize_t totalBytesRead = 0;

    while ((bytesRead = read(fd, responseHeader + totalBytesRead, sizeof(responseHeader) - totalBytesRead - 1)) > 0) {
        totalBytesRead += bytesRead;
        printf("Read %zd bytes\n", bytesRead);
        responseHeader[totalBytesRead] = '\0';
        if (strstr(responseHeader, "\r\n\r\n")) break;
    }

    if (bytesRead < 0) {
        perror("Failed to read from server");
        return;
    } else if (bytesRead == 0 && totalBytesRead == 0) {
        printf("No data received. Server may have closed the connection.\n");
        return;
    } else if (bytesRead == 0) {
        printf("End of data. Server closed the connection.\n");
    }

    printf("Headers received:\n%s\n", responseHeader);

    long contentLength = -1;
    char *clHeader = strstr(responseHeader, "Content-Length: ");
    if (clHeader) {
        sscanf(clHeader, "Content-Length: %ld", &contentLength);
    }

    if (contentLength > 0) {
        char *responseBody = malloc(contentLength + 1);
        if (!responseBody) {
            perror("Failed to allocate memory for response body");
            return;
        }

        totalBytesRead = 0;
        while (totalBytesRead < contentLength) {
            bytesRead = read(fd, responseBody + totalBytesRead, contentLength - totalBytesRead);
            if (bytesRead <= 0) {
                if (bytesRead < 0) perror("Error reading response body");
                break;
            }
            totalBytesRead += bytesRead;
        }

        responseBody[totalBytesRead] = '\0';
        printf("Body of the response:\n%s\n", responseBody);
        free(responseBody);
    } else {
        printf("Unable to read the response body.\n");
    }
}

int connect_to_server(struct hostent *host_entry)
{
   int fd;
   struct sockaddr_in their_addr;

   if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      return -1;
   }
   
   their_addr.sin_family = AF_INET;
   their_addr.sin_port = htons(PORT);
   their_addr.sin_addr = *((struct in_addr *)host_entry->h_addr);

   if (connect(fd, (struct sockaddr *)&their_addr,
      sizeof(struct sockaddr)) == -1)
   {
      close(fd);
      perror(0);
      return -1;
   }

   return fd;
}

struct hostent *gethost(char *hostname)
{
   struct hostent *he;

   if ((he = gethostbyname(hostname)) == NULL)
   {
      herror(hostname);
   }

   return he;
}

int main(int argc, char *argv[])
{
    validate_arguments(argc, argv);
    struct hostent *host_entry = gethost(argv[SERVER_ARG_IDX]);

    if (host_entry)
    {
        int fd = connect_to_server(host_entry);
        if (fd != -1)
        {
            send_request(fd);
            close(fd);
        }
    }

    return 0;
}
