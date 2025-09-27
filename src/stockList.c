#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stockList.h" 

void displayStockList() { 
    FILE *fp = fopen("./data/stocklist.csv", "r");
    if (!fp) {
        fprintf(stderr, "에러: stocklist.csv 파일을 찾을 수 없습니다.\n");
        return;
    }

    char line[256];
    int index = 1;

    system("clear");
    printf("--- 종목 목록 ---\n\n");

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;

        char *symbol = strtok(line, ",");
        char *name = strtok(NULL, ",");

        if (symbol && name) {
            printf("%2d. %-10s | %s\n", index++, symbol, name);
        }
    }

    printf("\n");
    fclose(fp);
}