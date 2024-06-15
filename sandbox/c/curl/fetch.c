#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Callback function to write data received from the server
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    // Append the received data to the user-provided buffer
    strncat((char *)userdata, (char *)ptr, total_size);
    return total_size;
}

int main(void) {
    CURL *easy_handle;
    CURLM *multi_handle;
    CURLMcode multi_code;
    int still_running = 0; // keep number of running handles
    char data[1024] = {0}; // buffer to store fetched data

    // Initialize the curl library
    curl_global_init(CURL_GLOBAL_ALL);

    // Create easy handle
    easy_handle = curl_easy_init();
    if (!easy_handle) {
        fprintf(stderr, "Failed to create easy handle\n");
        return 1;
    }

    // Set URL for the easy handle
    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://httpbin.org/get");

    // Set callback function to handle data
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_callback);

    // Set user data for callback function
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, data);

    // Create multi handle
    multi_handle = curl_multi_init();
    if (!multi_handle) {
        fprintf(stderr, "Failed to create multi handle\n");
        curl_easy_cleanup(easy_handle);
        return 1;
    }

    // Add easy handle to multi handle
    curl_multi_add_handle(multi_handle, easy_handle);

    // Perform the request
    multi_code = curl_multi_perform(multi_handle, &still_running);
    while (still_running) {
        int numfds;
        multi_code = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);
        if (multi_code != CURLM_OK) {
            fprintf(stderr, "curl_multi_wait() failed, code %d.\n", multi_code);
            break;
        }
        multi_code = curl_multi_perform(multi_handle, &still_running);
    }

    // Check for errors
    if (multi_code != CURLM_OK) {
        fprintf(stderr, "curl_multi_perform() failed, code %d.\n", multi_code);
    }

    // Print fetched data
    printf("Fetched Data: %s\n", data);

    // Clean up
    curl_multi_remove_handle(multi_handle, easy_handle);
    curl_easy_cleanup(easy_handle);
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();

    return 0;
}
