#include <stdio.h>
#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int main(void) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char *url = "http://api.openweathermap.org/data/2.5/weather?q=London&appid=cee21915a18477fe9d5b24eddb403a78";  // API key updated here
    char outfilename[FILENAME_MAX] = "output.json";

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename, "wb");
        if (fp == NULL) {
            fprintf(stderr, "Failed to open file %s for writing\n", outfilename);
            return 1;  // Return an error if file opening failed
        }
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        fclose(fp);
    } else {
        fprintf(stderr, "Failed to initialize curl.\n");
        return 1;  // Return an error if CURL initialization failed
    }
    return 0;
}
