#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <stdio.h>

typedef struct MemoryStruct {
    char *memory;
    size_t size;
} MemoryStruct;

// 콜백 함수 선언
size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

// 메인 함수 선언
int fetchAndSaveData(const char* symbol, const char* apiKey);

#endif