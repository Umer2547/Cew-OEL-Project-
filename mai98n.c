#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environment_monitor.h"

// Initialize libcurl
void init_curl(void) {
    curl_global_init(CURL_GLOBAL_ALL);
}

// Clean up libcurl
void cleanup_curl(void) {
    curl_global_cleanup();
}

// Callback function for libcurl's data fetching
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realSize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realSize + 1);
    if (!ptr) {
        printf("Out of memory!\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0;
    return realSize;
}

// Fetch data from API
void fetch_data(const char* url, struct MemoryStruct *chunk) {
    CURL *curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
}

// Process the JSON data
void process_data(const char *memory) {
    cJSON *json = cJSON_Parse(memory);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error in cJSON_Parse: %s\n", error_ptr);
        }
        return;
    }
    cJSON *main = cJSON_GetObjectItemCaseSensitive(json, "main");
    cJSON *temp = cJSON_GetObjectItemCaseSensitive(main, "temp");
    if (cJSON_IsNumber(temp)) {
        printf("\nCurrent Environmental Data:\n");
        printf("============================\n");
        printf("Temperature: %.2f°C\n", temp->valuedouble);
        file_store("Temperature", temp->valuedouble);
        alert_high_temp(temp->valuedouble);
    }
    cJSON_Delete(json);
}

// Store data in a file
void file_store(const char *info, double data) {
    FILE *file = fopen("environment_data.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s: %.2f°C\n", info, data);
        fclose(file);
    }
}

// Alert if temperature is too high
void alert_high_temp(double temp) {
    if (temp > 30.0) {
        printf("High temperature alert: %.2f°C\n", temp);
    }
}

// Main function
int main(void) {
    struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };
    if (chunk.memory == NULL) {
        printf("Failed to allocate memory.\n");
        return 1;
    }

    init_curl();
    const char *api_url = "http://api.openweathermap.org/data/2.5/weather?q=London&appid=462d97fc21e7b065ca22e57eb28b1d1&units=metric";
    printf("Fetching data...\n");
    fetch_data(api_url, &chunk);
    if (chunk.size > 0) {
        process_data(chunk.memory);
    } else {
        printf("No data received. Please check your network connection or API URL and Key.\n");
    }
    free(chunk.memory);
    cleanup_curl();

    return 0;
}
