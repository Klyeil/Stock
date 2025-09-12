#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <stdio.h>

size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

int fetchAndSaveData(const char* symbol, const char* apiKey);

#endif