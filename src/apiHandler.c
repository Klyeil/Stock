#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "apiHandler.h"
#include <sys/stat.h>
#include <time.h>

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
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);

    time_t now = time(NULL);
    struct stat fileStat;
    const double cacheDuration = 12 * 3600;

    if (stat(filePath, &fileStat) == 0) {
        double secondsSinceUpdate = difftime(now, fileStat.st_mtime);
        
        if (secondsSinceUpdate < cacheDuration) {
            printf("'%s'은(는) 12시간 이내에 업데이트되었습니다. (캐시 사용)\n", symbol);
            return 0;
        }
    }

    printf("'%s'의 최신 데이터를 API에서 가져옵니다...\n", symbol);

    CURL *curlHandle;
    CURLcode res;
    int returnCode = -1;
    
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

        res = curl_easy_perform(curlHandle);

        if (res != CURLE_OK) {
            fprintf(stderr, "데이터 요청 실패: %s\n", curl_easy_strerror(res));
            returnCode = -1;
        } else {
            if (strstr(chunk.memory, "Error Message") || strstr(chunk.memory, "Information")) {
                 printf("API로부터 에러 또는 정보 메시지를 받았습니다:\n%s\n", chunk.memory);
                 returnCode = -1;
            } else {
                printf("\n총 %zu 바이트 데이터 수신 완료.\n", chunk.size);
                FILE *outputFile = fopen(filePath, "wb");
                if (outputFile) {
                    fwrite(chunk.memory, 1, chunk.size, outputFile);
                    fclose(outputFile);
                    printf("데이터를 %s 파일에 성공적으로 저장했습니다.\n", filePath);
                    returnCode = 1;
                } else {
                    returnCode = -1;
                }
            }
        }

        curl_easy_cleanup(curlHandle);
        free(chunk.memory);
    } else {
        fprintf(stderr, "curl 핸들 초기화 실패.\n");
        returnCode = -1;
    }

    curl_global_cleanup();
    return returnCode;
}

double fetchCurrentPrice(const char* symbol, const char* apiKey) {
    CURL *curlHandle;
    CURLcode res;
    double price = -1.0;
    
    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), 
             "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=%s&apikey=%s",
             symbol, apiKey);

    curl_easy_init();
    curlHandle = curl_easy_init();
    if (curlHandle) {
        curl_easy_setopt(curlHandle, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curlHandle);

        if (res == CURLE_OK) {
            json_error_t error;
            json_t *root = json_loads(chunk.memory, 0, &error);

            if (root) {
                json_t *globalQuote = json_object_get(root, "Global Quote");
                if (json_is_object(globalQuote)) {
                    json_t *priceObj = json_object_get(globalQuote, "05. price");
                    if (json_is_string(priceObj)) {
                        price = atof(json_string_value(priceObj));
                    }
                }
                json_decref(root);
            }
        }
        curl_easy_cleanup(curlHandle);
        free(chunk.memory);
    }
    return price;
}