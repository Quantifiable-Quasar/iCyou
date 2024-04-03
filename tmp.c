#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to hold response data
struct ResponseData {
    char *data;
    size_t size;
};

// Callback function to handle the response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    struct ResponseData *response = (struct ResponseData *)userdata;
    size_t total_size = size * nmemb;
    
    // Reallocate memory for response data
    response->data = realloc(response->data, response->size + total_size + 1);
    if (response->data == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }
    
    // Copy response data to buffer
    memcpy(&(response->data[response->size]), ptr, total_size);
    response->size += total_size;
    response->data[response->size] = '\0'; // Null-terminate the string
    
    return total_size;
}

// Function to perform HTTP GET request and return response data
char *curl_request(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize libcurl\n");
        return NULL;
    }

    struct ResponseData response = { .data = NULL, .size = 0 };
    
    // Set the URL to retrieve
    curl_easy_setopt(curl, CURLOPT_URL, url);
    
    // Set the callback function to handle the response data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the HTTP GET request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to perform HTTP request: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(response.data); // Free memory allocated for response data
        return NULL;
    }

    // Cleanup
    curl_easy_cleanup(curl);
    
    return response.data;
}

int main() {
    char *response = curl_request("https://www.google.com");
    if (response) {
        printf("Response:\n%s\n", response);
        free(response); // Free memory allocated for response data
    }
    return 0;
}

