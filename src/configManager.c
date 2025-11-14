#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configManager.h"

void loadApiKey(char* apiKeyBuffer, int bufferSize) {
    FILE* fp = fopen("config.ini", "r");
    if (!fp) {
        fprintf(stderr, "치명적 에러: config.ini 파일을 찾을 수 없습니다.\n");
        fprintf(stderr, "프로그램을 종료합니다. config.ini 파일을 생성해주세요.\n");
        exit(1);
    }

    char line[256];
    if (fgets(line, sizeof(line), fp)) {
        char* key = strtok(line, "=");
        char* value = strtok(NULL, "\n");
        
        if (key && value && strcmp(key, "API_KEY") == 0) {
            strncpy(apiKeyBuffer, value, bufferSize - 1);
            apiKeyBuffer[bufferSize - 1] = '\0';
        }
    } else {
        fprintf(stderr, "치명적 에러: config.ini 파일에서 API_KEY를 읽을 수 없습니다.\n");
        exit(1);
    }

    fclose(fp);
}