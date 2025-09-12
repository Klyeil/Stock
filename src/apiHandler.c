#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "apiHandler.h"

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realSize = size * nmemb;
    FILE *fp = (FILE *)userp;
    
    if (fp == NULL) {
        fprintf(stderr, "Error: File pointer is null in callback.\n");
        return 0;
    }
    
    return fwrite(contents, size, nmemb, fp);
}

int fetchAndSaveData(const char* symbol, const char* apiKey) {
    CURL *curlHandle;
    CURLcode res;
    
    char apiUrl[256];
    char filePath[256];
    FILE *outputFile;

    snprintf(apiUrl, sizeof(apiUrl), 
             "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY_ADJUSTED&symbol=%s&apikey=%s&datatype=csv",
             symbol, apiKey);
    
    snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);

    curl_global_init(CURL_GLOBAL_ALL);
    curlHandle = curl_easy_init();

    if (curlHandle) {
        outputFile = fopen(filePath, "wb");
        if (outputFile == NULL) {
            fprintf(stderr, "Could not open file %s for writing.\n", filePath);
            curl_easy_cleanup(curlHandle);
            return -1;
        }

        curl_easy_setopt(curlHandle, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, outputFile);

        printf("Fetching data from: %s\n", apiUrl);
        res = curl_easy_perform(curlHandle);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Data for %s successfully saved to %s\n", symbol, filePath);
        }

        curl_easy_cleanup(curlHandle);
        fclose(outputFile);
    } else {
        fprintf(stderr, "curl_easy_init() failed.\n");
        return -1;
    }

    curl_global_cleanup();
    return 0;
}