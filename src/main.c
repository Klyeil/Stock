#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apiHandler.h"
#include "dataProcessor.h"
#include "backTester.h"
#include "apiSearch.h"
#include "chartPlotter.h"
#include "stockList.h"



static DailyData* lastAnalyzedData = NULL;
static int lastDataCount = 0;
static char lastSymbol[16] = {0};

void pressEnterToContinue() {
    printf("\n계속하려면 Enter 키를 누르세요...");
    while(getchar() != '\n');
    getchar();
}

void printMainMenu() {
    system("clear");
    printf("=========================================\n");
    printf("=   C언어 주식 분석 및 백테스팅 플랫폼   =\n");
    printf("=========================================\n");
    if (lastDataCount > 0) {
        printf("  [현재 분석된 종목: %s]\n", lastSymbol);
    }
    printf("\n");
    printf("  1. 종목 분석하기\n");
    printf("  2. 종목 검색하기\n");
    printf("  3. 종목 목록 보기\n");
    if (lastDataCount > 0) {
        printf("  4. 마지막 분석 차트 보기\n");
    }
    printf("  0. 프로그램 종료\n");
    printf("\n");
    printf("=========================================\n");
    printf("선택: ");
}

void viewStockList() {
    displayStockList();
    pressEnterToContinue();
}

void analyzeStock() {
    const char* apiKey = "YOUR_API_KEY";
    char currentSymbol[16];

    system("clear");
    printf("## 새로운 종목 분석 ##\n");
    printf("분석할 미국 주식 종목 코드를 입력하세요 (예: IBM, AAPL): ");
    scanf("%s", currentSymbol);


    if (lastAnalyzedData != NULL) {
        freeData(lastAnalyzedData);
        lastAnalyzedData = NULL;
        lastDataCount = 0;
    }

    if (fetchAndSaveData(currentSymbol, apiKey) == 0) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", currentSymbol);

        int dataCount = loadCsvFile(filePath, &lastAnalyzedData);

        if (dataCount > 0) {
            lastDataCount = dataCount;
            strcpy(lastSymbol, currentSymbol);

            calculateSma(lastAnalyzedData, lastDataCount, 5);
            calculateSma(lastAnalyzedData, lastDataCount, 20);
            runBacktest(lastAnalyzedData, lastDataCount);

        }
    }
    pressEnterToContinue();
}

void viewChart() {
    if (lastAnalyzedData == NULL) {
        printf("먼저 종목 분석을 실행해주세요.\n");
    } else {
        plotChart(lastAnalyzedData, lastDataCount);
    }
    pressEnterToContinue();
}

void searchStock() {
    char keyword[50];
    const char* apiKey = "YOUR_API_KEY";

    system("clear");
    printf("## 종목 검색 ##\n");
    printf("검색할 키워드를 입력하세요 (예: apple, microsoft): ");
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
            case 3:
                viewStockList();
                break;
            case 4:
                if (lastDataCount > 0) {
                    viewChart();
                } else {
                    printf("잘못된 선택입니다. 다시 입력해주세요.\n");
                    pressEnterToContinue();
                }
                break;
            case 0:
                printf("프로그램을 종료합니다.\n");
                if (lastAnalyzedData != NULL) {
                    freeData(lastAnalyzedData);
                }
                return 0;
            default:
                printf("잘못된 선택입니다. 다시 입력해주세요.\n");
                pressEnterToContinue();
                break;
        }
    }

    return 0;
}