#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
#include "configManager.h"

static DailyData* lastAnalyzedData = NULL;
static int lastDataCount = 0;
static char lastSymbol[16] = {0};

void printMainMenu(const Portfolio* portfolio) {
    char dateStr[20];
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localTime);

    system("clear");
    printf("=========================================\n");
    printf("=   C언어 주식 포트폴리오 & 분석 플랫폼 =\n");
    printf("=   (오늘 날짜: %s)   =\n", dateStr);
    printf("=========================================\n\n");

    printf("  [실전! 모의 투자]\n");
    printf("  1. 내 포트폴리오 보기/관리 (현재가 기준)\n\n");
    
    printf("  [연습! 과거 시뮬레이션]\n");
    printf("  2. 트레이딩 시뮬레이션 시작하기 (종목 분석 필요)\n\n");
    
    printf("  [분석 도구]\n");
    printf("  3. 추천 종목 자동 분석 (스크리너)\n");
    printf("  4. 단일 종목 수동 분석 (백테스팅)\n");
    printf("  5. 종목 검색하기\n");
    printf("  6. 종목 목록 보기\n");
    printf("  7. 주식 그래프 보기 (종목 코드 입력)\n");
    
    printf("\n  0. 저장하고 종료\n");
    printf("=========================================\n");
    printf("선택: ");
}

void analyzeStock(const char* apiKey) {
    char currentSymbol[16];
    int shortPeriod = 0, longPeriod = 0;

    system("clear");
    printf("## 단일 종목 수동 분석 ##\n");
    printf("분석할 주식 종목 코드를 입력하세요 (예: IBM, AAPL): ");
    getStringInput(currentSymbol, sizeof(currentSymbol));

    while (shortPeriod <= 0) {
        printf("단기 이동평균 기간(일)을 입력하세요 (예: 5, 10): ");
        shortPeriod = getIntegerInput();
        if (shortPeriod <= 0) printf("기간은 0보다 커야 합니다.\n");
    }
    while (longPeriod <= shortPeriod) {
        printf("장기 이동평균 기간(일)을 입력하세요 (예: 20, 60): ");
        longPeriod = getIntegerInput();
        if (longPeriod <= shortPeriod) printf("장기 기간은 단기 기간(%d일)보다 길어야 합니다.\n", shortPeriod);
    }

    if (lastAnalyzedData != NULL) {
        freeData(lastAnalyzedData);
        lastAnalyzedData = NULL;
        lastDataCount = 0;
    }

    int fetchResult = fetchAndSaveData(currentSymbol, apiKey);
    if (fetchResult >= 0) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", currentSymbol);
        int dataCount = loadCsvFile(filePath, &lastAnalyzedData);

        if (dataCount > 0) {
            lastDataCount = dataCount;
            strcpy(lastSymbol, currentSymbol);

            calculateSma(lastAnalyzedData, lastDataCount, shortPeriod, 1);
            calculateSma(lastAnalyzedData, lastDataCount, longPeriod, 0);
            
            SimulationState finalState = runBacktest(lastAnalyzedData, lastDataCount);
            printDetailedReport(finalState, lastAnalyzedData, lastDataCount, shortPeriod, longPeriod);
        }
    }
    pressEnterToContinue();
}

void searchStock(const char* apiKey) {
    char keyword[50];
    system("clear");
    printf("## 종목 검색 ##\n");
    printf("검색할 키워드를 입력하세요 (예: apple, microsoft): ");
    getStringInput(keyword, sizeof(keyword));
    searchSymbols(keyword, apiKey);
    pressEnterToContinue();
}

void viewStockList() {
    displayStockList();
    pressEnterToContinue();
}

void viewChartStandalone(const char* apiKey) {
    char symbol[16];
    system("clear");
    printf("## 주식 그래프 보기 ##\n");
    printf("차트를 볼 종목 코드를 입력하세요 (예: IBM, AAPL): ");
    getStringInput(symbol, sizeof(symbol));

    int fetchResult = fetchAndSaveData(symbol, apiKey);
    if (fetchResult >= 0) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);

        DailyData* chartData = NULL;
        int dataCount = loadCsvFile(filePath, &chartData);

        if (dataCount > 0) {
            plotChart(chartData, dataCount);
            freeData(chartData);
        }
    } else {
        printf("'%s' 종목의 데이터를 가져오는 데 실패했습니다.\n", symbol);
    }
    pressEnterToContinue();
}

int main() {
    Portfolio portfolio;
    char apiKey[128];

    loadApiKey(apiKey, sizeof(apiKey));
    loadPortfolio(&portfolio);
    
    int choice = -1;
    while (1) {
        printMainMenu(&portfolio);
        choice = getIntegerInput();

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
            case 3: {
                int shortPeriod = 0, longPeriod = 0;
                system("clear");
                printf("## 추천 종목 자동 분석 (스크리너) ##\n");

                while (shortPeriod <= 0) {
                    printf("분석에 사용할 단기 이동평균 기간(일)을 입력하세요 (예: 5, 10): ");
                    shortPeriod = getIntegerInput();
                    if (shortPeriod <= 0) printf("기간은 0보다 커야 합니다.\n");
                }
                while (longPeriod <= shortPeriod) {
                    printf("분석에 사용할 장기 이동평균 기간(일)을 입력하세요 (예: 20, 60): ");
                    longPeriod = getIntegerInput();
                    if (longPeriod <= shortPeriod) printf("장기 기간은 단기 기간(%d일)보다 길어야 합니다.\n", shortPeriod);
                }
                
                runScreener(apiKey, shortPeriod, longPeriod);
                break;
            }
            case 4:
                analyzeStock(apiKey);
                break;
            case 5:
                searchStock(apiKey);
                break;
            case 6:
                viewStockList();
                break;
            case 7:
                viewChartStandalone(apiKey);
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