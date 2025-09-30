#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <stdio.h>

// API 응답을 저장할 메모리 구조체 정의
typedef struct MemoryStruct {
    char *memory;
    size_t size;
} MemoryStruct;

// libcurl이 데이터를 받을 때마다 호출할 공용 콜백 함수
size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

// 특정 종목의 과거 데이터를 API로 가져와서 파일에 저장하는 함수
int fetchAndSaveData(const char* symbol, const char* apiKey);

double fetchCurrentPrice(const char* symbol, const char* apiKey);

#endif