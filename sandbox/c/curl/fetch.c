#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Callback function to write data received from the server
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    fwrite(ptr, size, nmemb, stdout);
    return total_size;
}

int fetch_once(CURLM *multi_handle) {
    CURLMcode multi_code;
    int numfds;
    int running_handles;
    multi_code = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);
    if (multi_code != CURLM_OK) {
        fprintf(stderr, "curl_multi_wait() failed, code %d.\n", multi_code);
        return 0;
    }
    multi_code = curl_multi_perform(multi_handle, &running_handles);
    if (multi_code != CURLM_OK) {
        fprintf(stderr, "curl_multi_perform() failed, code %d.\n", multi_code);
        return 0;
    }
    return running_handles;
}

int main(void) {
    CURL *easy_handle;
    CURLM *multi_handle;
    int still_running = 0; // keep number of running handles

    // Initialize the curl library
    curl_global_init(CURL_GLOBAL_ALL);

    // Create easy handle
    easy_handle = curl_easy_init();
    if (!easy_handle) {
        fprintf(stderr, "Failed to create easy handle\n");
        return 1;
    }

    // Set URL for the easy handle
    const char * URL = "https://httpbin.org/get";
    curl_easy_setopt(easy_handle, CURLOPT_URL, URL);

    // Set callback function to handle data
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_callback);

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
    do {
        still_running = fetch_once(multi_handle);

        long code;
        curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &code);
        fprintf(stderr, "status code %d\n", code);

        curl_off_t content_length;
        curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &content_length);
        fprintf(stderr, "content length %lld\n", content_length);
    } while (still_running);

    // Clean up
    curl_multi_remove_handle(multi_handle, easy_handle);
    curl_easy_cleanup(easy_handle);
    curl_multi_cleanup(multi_handle);
    curl_global_cleanup();

    return 0;
}
