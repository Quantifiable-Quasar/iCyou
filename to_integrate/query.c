#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// curl response struct
struct curlResponse {
        char *data;
        size_t size;
};


// this puts the response in the struct for later access
size_t cb(void *ptr, size_t size, size_t nmemb, void *userdata){
// https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
    size_t realsize = size * nmemb;
    struct curlResponse *response = (struct curlResponse *)userdata;

    // wow! dynamic memory allocation!
    response->data = realloc(response->data, response->size + realsize + 1);

    // error checking
    if (response->data == NULL){
            fprintf(stderr, "Out of memory!\n");
            return 0;
    }


    // put data in struct
    memcpy(&(response->data[response->size]), ptr, realsize);
    response->size += realsize;
    response->data[response->size] = '\0';

    // only need to return size bc data is stored in struct by pointer
    return realsize;
}

// this function calls curl for a GET request to url specified in param
char *curl_get(const char *url) {
// https://curl.se/libcurl/c/https.html

        // initialize curl
        CURL *curl = curl_easy_init();
        if (!curl) {
                fprintf(stderr, "Libcurl init failed\n");
                return NULL;
        }

        // initialize response struct
        struct curlResponse response = {.data = NULL, .size = 0};

        // set some options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // check to make sure curl worked
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
                fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                free(response.data);
                return NULL;
        }

        // clean and quit
        curl_easy_cleanup(curl);

        return response.data;
}


int main(){
    char *response = curl_get("https://google.com");
    if (response) {
            printf("Website Response:\n%s\n", response);
            free(response);
    }
    return 0;
}
