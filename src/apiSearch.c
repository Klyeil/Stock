#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "apiHandler.h"
#include "apiSearch.h"

int searchSymbols(const char* keywords, const char* apiKey) {
    CURL *curlHandle;
    CURLcode res;
    
    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), 
             "https://www.alphavantage.co/query?function=SYMBOL_SEARCH&keywords=%s&apikey=%s",
             keywords, apiKey);

    curl_global_init(CURL_GLOBAL_ALL);
    curlHandle = curl_easy_init();

    if (curlHandle) {
        curl_easy_setopt(curlHandle, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void *)&chunk);

        printf("'%s' 키워드로 종목 검색 중...\n", keywords);
        res = curl_easy_perform(curlHandle);

        if (res != CURLE_OK) {
            fprintf(stderr, "데이터 요청 실패: %s\n", curl_easy_strerror(res));
        } else {
            json_error_t error;
            json_t *root = json_loads(chunk.memory, 0, &error);

            if (!root) {
                fprintf(stderr, "JSON 파싱 에러: %s (line: %d, column: %d)\n", error.text, error.line, error.column);
            } else {
                json_t *bestMatches = json_object_get(root, "bestMatches");
                if (json_is_array(bestMatches)) {
                    size_t count = json_array_size(bestMatches);
                    printf("\n--- 검색 결과 (%zu개) ---\n", count);
                    for (size_t i = 0; i < count; i++) {
                        json_t *match = json_array_get(bestMatches, i);
                        const char *symbol = json_string_value(json_object_get(match, "1. symbol"));
                        const char *name = json_string_value(json_object_get(match, "2. name"));
                        const char *region = json_string_value(json_object_get(match, "4. region"));
                        
                        printf("%zu. %s (%s) - %s\n", i + 1, symbol, name, region);
                    }
                }
                json_decref(root);
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