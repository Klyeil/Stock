#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apiHandler.h"
#include "dataProcessor.h"
#include "backTester.h"
#include "apiSearch.h"
#include "chartPlotter.h"
#include "stockList.h"
#include "virtualTrader.h"
#include "mainHelper.h"
#include "screener.h"
#include "portfolioManager.h"

static DailyData* lastAnalyzedData = NULL;
static int lastDataCount = 0;
static char lastSymbol[16] = {0};

void printMainMenu(const Portfolio* portfolio) {
    system("clear");
    printf("=========================================\n");
    printf("=   C언어 주식 포트폴리오 & 분석 플랫폼  =\n");
    printf("=========================================\n\n");

    printf("  [실전 모의 투자]\n");
    printf("  1. 내 포트폴리오 보기/관리 (실시간)\n\n");
    
    printf("  [연습 과거 시뮬레이션]\n");
    printf("  2. 트레이딩 시뮬레이션 시작하기\n\n");
    
    printf("  [분석 도구]\n");
    printf("  3. 추천 종목 자동 분석 (스크리너)\n");
    printf("  4. 단일 종목 수동 분석 (백테스팅)\n");
    printf("  5. 종목 검색하기 / 6. 종목 목록 보기\n\n");
    
    printf("  0. 저장하고 종료\n");
    printf("=========================================\n");
    printf("선택: ");
}

void analyzeStock(const char* apiKey) {
    char currentSymbol[16];

    system("clear");
    printf("## 단일 종목 수동 분석 ##\n");
    printf("분석할 주식 종목 코드를 입력하세요 (예: IBM, AAPL): ");
    scanf("%s", currentSymbol);
    while(getchar() != '\n');

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
            SimulationState finalState = runBacktest(lastAnalyzedData, lastDataCount);
            printDetailedReport(finalState, lastAnalyzedData, lastDataCount);
        }
    }
    pressEnterToContinue();
}

void searchStock(const char* apiKey) {
    char keyword[50];
    system("clear");
    printf("## 종목 검색 ##\n");
    printf("검색할 키워드를 입력하세요 (예: apple, microsoft): ");
    scanf("%s", keyword);
    while(getchar() != '\n');
    searchSymbols(keyword, apiKey);
    pressEnterToContinue();
}

void viewStockList() {
    displayStockList();
    pressEnterToContinue();
}

void viewChart() {
    if (lastAnalyzedData == NULL) {
        printf("먼저 '4. 단일 종목 수동 분석'을 실행해주세요.\n");
    } else {
        plotChart(lastAnalyzedData, lastDataCount);
    }
    pressEnterToContinue();
}

int main() {
    Portfolio portfolio;
    loadPortfolio(&portfolio);
    
    const char* apiKey = "W46WANT3781FTIOS";

    int choice = -1;
    while (1) {
        printMainMenu(&portfolio);
        scanf("%d", &choice);
        while(getchar() != '\n');

        switch (choice) {
            case 1:
                managePortfolio(&portfolio, apiKey);
                break;
            case 2:
                if (lastDataCount > 0) {
                    startVirtualTrading(lastAnalyzedData, lastDataCount, &portfolio);
                } else {
                    printf("먼저 '4. 단일 종목 수동 분석'을 실행하여 시뮬레이션할 데이터를 로드해주세요.\n");
                    pressEnterToContinue();
                }
                break;
            case 3:
                runScreener();
                break;
            case 4:
                analyzeStock(apiKey);
                break;
            case 5:
                searchStock(apiKey);
                break;
            case 6:
                viewStockList();
                break;
            case 0:
                printf("프로그램을 종료합니다.\n");
                savePortfolio(&portfolio);
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