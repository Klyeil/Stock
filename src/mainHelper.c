#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mainHelper.h"

static void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
void pressEnterToContinue() {
    printf("\n계속하려면 Enter 키를 누르세요...");
    clearInputBuffer();
}

int getIntegerInput() {
    char buffer[20];
    long choice = -1;
    char *endptr;

    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            choice = strtol(buffer, &endptr, 10);
            if (endptr == buffer || *endptr != '\0') {
                printf("잘못된 입력입니다. 숫자만 입력해주세요: ");
            } else {
                return (int)choice;
            }
        } else {

            printf("입력 오류입니다. 다시 시도해주세요: ");
            clearInputBuffer();
        }
    }
}

void getStringInput(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}