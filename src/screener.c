#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "screener.h"
#include "apiHandler.h"
#include "dataProcessor.h"
#include "backTester.h"
#include "mainHelper.h"

typedef struct ScreenerResult {
    char symbol[16];
    double totalReturn;
} ScreenerResult;


int compareResults(const void* a, const void* b) {
    ScreenerResult* resultA = (ScreenerResult*)a;
    ScreenerResult* resultB = (ScreenerResult*)b;
    if (resultB->totalReturn > resultA->totalReturn) return 1;
    if (resultB->totalReturn < resultA->totalReturn) return -1;
    return 0;
}

void runScreener(const char* apiKey) {
    FILE *fp = fopen("data/screenerList.csv", "r");
    if (!fp) {
        fprintf(stderr, "에러: screenerList.csv 파일을 찾을 수 없습니다.\n");
        pressEnterToContinue();
        return;
    }

    system("clear");
    printf("--- 자동 종목 스크리닝을 시작합니다 ---\n");
    printf("API 호출 시 15초씩 대기합니다...\n\n");

    ScreenerResult results[100];
    int resultCount = 0;
    char line[256];
    
    const int shortPeriod = 5;
    const int longPeriod = 20;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;
        char *symbol = strtok(line, ",");
        char *name = strtok(NULL, ",");

        if (!symbol || !name) continue;
        
        printf("분석 중 (%d)... %s (%s)\n", resultCount + 1, name, symbol);

        int fetchResult = fetchAndSaveData(symbol, apiKey);

        if (fetchResult >= 0) {
            char filePath[256];
            snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);

            DailyData* stockData = NULL;
            int dataCount = loadCsvFile(filePath, &stockData);

            if (dataCount > 0) {
                calculateSma(stockData, dataCount, shortPeriod, 1);
                calculateSma(stockData, dataCount, longPeriod, 0);
                
                SimulationState finalState = runBacktest(stockData, dataCount);
                double finalEquity = finalState.cash + (finalState.shares * stockData[0].close);
                double totalReturn = ((finalEquity - finalState.initialCash) / finalState.initialCash) * 100;
                
                strcpy(results[resultCount].symbol, symbol);
                results[resultCount].totalReturn = totalReturn;
                resultCount++;
                
                freeData(stockData);
            }
        }
        
        if (fetchResult == 1) {
            printf("15초 대기...\n\n");
            sleep(15);
        } else {
            printf("\n");
        }
    }
    fclose(fp);

    qsort(results, resultCount, sizeof(ScreenerResult), compareResults);

    system("clear");
    printf("--- 자동 분석 결과 TOP 10 ---\n\n");
    printf("기준 전략: %d일/%d일 이동평균선 교차\n", shortPeriod, longPeriod);
    printf("----------------------------------\n");
    printf(" 순위 | 종목코드   | 과거 수익률\n");
    printf("----------------------------------\n");
    for (int i = 0; i < 10 && i < resultCount; i++) {
        printf(" %2d위 | %-10s | %8.2f%%\n", i + 1, results[i].symbol, results[i].totalReturn);
    }
    printf("----------------------------------\n");
    pressEnterToContinue();
}