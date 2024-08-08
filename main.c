#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_SERVER_PORT 8080
#define MAX_CONNECTIONS 10
#define MAX_REQUEST_SIZE 1024

int main(void)
{
    int server_port = DEFAULT_SERVER_PORT;
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char request[MAX_REQUEST_SIZE] = {0};

    int input_server_port;
    printf("Enter PORT to listen: ");
    if (scanf("%d", &input_server_port) == 1)
    {
        server_port = input_server_port;
    }

    // create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces (0.0.0.0)
    address.sin_port = htons(server_port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen to clients, with MAX_CONNECTIONS in accept queue
    if (listen(server_fd, MAX_CONNECTIONS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", server_port);

    // loop to wait for a connection and then accept it
    while (1)
    {
        // this call blocks until a connection is made
        printf("Waiting for a connection...\n");
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // read the request
        read(client_fd, request, MAX_REQUEST_SIZE);
        printf("Received request: %s\n", request);

        // send an http response including content type of text and content length
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello world!";
        write(client_fd, response, strlen(response));

        close(client_fd);
    }

    return 0;
}
