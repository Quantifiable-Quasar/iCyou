#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define INITIAL_BUFFER_SIZE 1024
/* 
 * print output for sucess as well as failure
 * command line options (help, verbose, port, etc)
 * maybe multithread the conenction funciton so many clients can acccess.
*/

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char *buffer = (char *)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        perror("Failed to allocate memory for buffer");
        exit(EXIT_FAILURE);
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Continuously accept connections
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Receive data
        ssize_t total_bytes_received = 0;
        ssize_t buffer_size = INITIAL_BUFFER_SIZE;
        ssize_t bytes_received;
        while ((bytes_received = read(new_socket, buffer + total_bytes_received, buffer_size - total_bytes_received)) > 0) {
            total_bytes_received += bytes_received;
            if (total_bytes_received == buffer_size) {
                // Resize the buffer
                buffer_size *= 2;
                buffer = (char *)realloc(buffer, buffer_size * sizeof(char));
                if (buffer == NULL) {
                    perror("Failed to reallocate memory for buffer");
                    exit(EXIT_FAILURE);
                }
            }
        }
        if (bytes_received < 0) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        printf("Received data from client: %s\n", buffer);
        close(new_socket);
    }

    free(buffer);
    close(server_fd);
    return 0;
}

