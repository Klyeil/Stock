#include <stdio.h>
#include <stdlib.h>
#include "apiHandler.h"
#include "dataProcessor.h"
#include "backTester.h"
#include "apiSearch.h"
#include "chartPlotter.h"


void pressEnterToContinue() {
    printf("\n계속하려면 Enter 키를 누르세요...");
    while(getchar() != '\n');
    getchar();
}

void printMainMenu() {
    system("clear");
    printf("=========================================\n");
    printf("========   C언어 주식 분석 프로그램   ========\n");
    printf("=========================================\n");
    printf("\n");
    printf("  1. 새로운 종목 분석하기\n");
    printf("  2. 종목 검색하기\n");
    printf("  0. 프로그램 종료\n");
    printf("\n");
    printf("=========================================\n");
    printf("선택: ");
}

void analyzeStock() {
    char symbol[10];
    const char* apiKey = "W46WANT3781FTIOS";

    system("clear");
    printf("## 새로운 종목 분석 ##\n");
    printf("분석할 미국 주식 종목 코드를 입력하세요 (예: IBM, AAPL): ");
    scanf("%s", symbol);

    if (fetchAndSaveData(symbol, apiKey) == 0) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);

        DailyData* stockData = NULL;
        int dataCount = loadCsvFile(filePath, &stockData);

        if (dataCount > 0) {
            calculateSma(stockData, dataCount, 5);
            calculateSma(stockData, dataCount, 20);
            runBacktest(stockData, dataCount);
            plotChart(stockData, dataCount);

            freeData(stockData);
        }
    }
    pressEnterToContinue();
}

void searchStock() {
    char keyword[50];
    const char* apiKey = "YOUR_API_KEY";

    system("clear");
    printf("## 종목 검색 ##\n");
    printf("검색할 키워드를 입력하세요 (예: samsung, apple, microsoft): ");
    scanf("%s", keyword);

    searchSymbols(keyword, apiKey);
    pressEnterToContinue();
}

int main() {
    int choice = -1;

    while (1) {
        printMainMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                analyzeStock();
                break;
            case 2:
                searchStock();
                break;
            case 0:
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 선택입니다. 다시 입력해주세요.\n");
                pressEnterToContinue();
                break;
        }
    }

    return 0;
}