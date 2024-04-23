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
void buildURL(char* package, char* version);
int wgetToFile(const char* url, const char* outfile);

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

//        printf("Received data from client: %s\n", buffer);

          
            char package[64];
            char version[64];
            char finURL[MAX_URL] = "";

            char* test = "https://example.com";
            char* outfile = "testing";
            curlToFile(test, outfile);
           

            char *line = strtok(buffer, "\n");
            while(line != NULL) {
                sscanf(line, "%s %s", package, version);
                printf("Package: %s, Version %s\n", package, version);
                
//                buildURL(package, version);
           //     curlToFile(test, package);
                  char finURL[MAX_URL];
                  sprintf(finURL, "https://services.nvd.nist.gov/rest/json/cves/2.0?virtualMatchString=cpe:2.3:a:*:%s:*:*:*:en", package);
                  wgetToFile(finURL, package);

                line = strtok(NULL, "\n");
            }

        close(new_socket);
    }

    free(buffer);
    close(server_fd);
    return 0;
}

int curlToFile(char* url, char* outfile) {
        CURL *curl;
        CURLcode res;

        printf("Curling url: %s\n", url);

        curl = curl_easy_init();
        if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url);
                
                char out[INITIAL_BUFFER_SIZE] = "pkgs/";
                strcat(out, outfile);
                FILE *fptr = fopen(out, "w");
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);

                res = curl_easy_perform(curl);

                if(res != CURLE_OK) {
                        fprintf(stderr, "Curl failed: %s\n", curl_easy_strerror(res));
                        return 1;
                }
        
                curl_easy_cleanup(curl);
        }
        return 0;
}


void buildURL(char* package, char* version) {

    char *delim = ".";
    char *line = strtok(version, delim);
    char finURL[MAX_URL] = "";

    char finver[10] = "";
    int counter = 1;
    
    while(counter <=2 && line != NULL) {
            int tmpver;
            sscanf(line, "%d", &tmpver);
            if(counter == 1) {
                    sprintf(finver + strlen(finver), "%d.", tmpver);
            } else {
                    sprintf(finver + strlen(finver), "%d", tmpver);
            }

            line = strtok(NULL, delim);
            counter++;
    }

    sprintf(finURL, "https://services.nvd.nist.gov/rest/json/cves/2.0?virtualMatchString=cpe:2.3:a:*:%s:%s:*:*:en", package, finver);
    printf("url: %s\n", finURL);
}


int wgetToFile(const char* url, const char* outfile) {
    char command[1024];
    snprintf(command, sizeof(command), "wget -O pkgs/%s %s", outfile, url);
    return system(command);
}

