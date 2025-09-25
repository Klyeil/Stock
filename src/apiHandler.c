#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "apiHandler.h"


size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realSize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realSize + 1);
    if (ptr == NULL) {
        printf("메모리 부족 (realloc이 NULL을 반환)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0;

    return realSize;
}

int fetchAndSaveData(const char* symbol, const char* apiKey) {
    CURL *curlHandle;
    CURLcode res;
    
    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), 
             "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol=%s&apikey=%s&datatype=csv",
             symbol, apiKey);

    curl_global_init(CURL_GLOBAL_ALL);
    curlHandle = curl_easy_init();

    if (curlHandle) {
        curl_easy_setopt(curlHandle, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void *)&chunk);

        printf("데이터 가져오는 중: %s\n", apiUrl);
        res = curl_easy_perform(curlHandle);

        if (res != CURLE_OK) {
            fprintf(stderr, "데이터 요청 실패: %s\n", curl_easy_strerror(res));
        } else {
            printf("\n총 %zu 바이트 데이터 수신 완료.\n", chunk.size);

            char filePath[256];
            snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);
            FILE *outputFile = fopen(filePath, "wb");
            if (outputFile) {
                fwrite(chunk.memory, 1, chunk.size, outputFile);
                fclose(outputFile);
                printf("데이터를 %s 파일에 성공적으로 저장했습니다.\n", filePath);
            }
        }

        curl_easy_cleanup(curlHandle);
        free(chunk.memory);
    } else {
        fprintf(stderr, "curl 핸들 초기화 실패.\n");
        return -1;
    }

    curl_global_cleanup();
    return 0;
}