#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define INITIAL_BUFFER_SIZE 1024
#define MAX_URL 128


int curlToFile(char* url, char* outfile);

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
//        Test statement to make sure packages are being recieved
//        printf("Received data from client: %s\n", buffer);

          
            char package[64];
            char version[64];

            char *line = strtok(buffer, "\n");
            while(line != NULL) {
                sscanf(line, "%s %s", package, version);
                printf("Package: %s, Version %s\n", package, version);
                curlToFile(package, version);

                sleep(6);
                
                line = strtok(NULL, "\n");
            }
        close(new_socket);
    }

    free(buffer);
    close(server_fd);
    return 0;
}
// Function to create directory if it doesn't exist
int createDirectory(const char* path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        // Directory doesn't exist, create it
        if (mkdir(path, 0777) == -1) {
            printf("Error: Failed to create directory %s\n", path);
            return -1;
        }
        printf("Directory created: %s\n", path);
    }
    return 0;
}

int curlToFile(char* package, char* version) {
    // Check if package and version pointers are valid
    if (package == NULL || version == NULL) {
        printf("Error: Invalid package or version pointer\n");
        return -1;
    }

    char url[INITIAL_BUFFER_SIZE] = "https://services.nvd.nist.gov/rest/json/cves/2.0?VirtualMatchString=cpe:2.3:*:*:";
    strcat(url, package);
    strcat(url, ":*:*:*:en");

    printf("url: %s\n", url);

    CURL* easyhandle = curl_easy_init();
    if (easyhandle == NULL) {
        printf("Error: Failed to initialize Curl\n");
        return -1;
    }

    // Set Curl options
    curl_easy_setopt(easyhandle, CURLOPT_URL, url);
    curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; rv:112.0) Gecko/20100101 Firefox/112.0");

    char outfile[INITIAL_BUFFER_SIZE] = "pkgs/";
    strcat(outfile, package);

    if (createDirectory("pkgs") == -1) {
        curl_easy_cleanup(easyhandle); 
        return -1;
    }

    FILE* file = fopen(outfile, "w");
    if (file == NULL) {
        printf("Error: Failed to open file %s\n", outfile);
        curl_easy_cleanup(easyhandle); 
        return -1;
    }

    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);

    // perform curl
    CURLcode res = curl_easy_perform(easyhandle);
    if (res != CURLE_OK) {
        printf("Error: Curl operation failed: %s\n", curl_easy_strerror(res));
        fclose(file); 
        curl_easy_cleanup(easyhandle);
        return -1;
    }

    fclose(file);
    curl_easy_cleanup(easyhandle);

    return 0;
}

