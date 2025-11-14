#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "virtualTrader.h"
#include "mainHelper.h"
#include "dataProcessor.h" 

void startVirtualTrading(DailyData* dataArray, int dataCount, Portfolio* portfolio) {
    double simCash = 10000.0;
    int simShares = 0;
    
    printf("\n--- 트레이딩 시뮬레이션(연습)을 시작합니다. ---\n");
    printf("분석 중인 종목을 대상으로 $10,000의 가상 자본으로 시작합니다.\n");
    pressEnterToContinue();
    
    for (int i = dataCount - 1; i >= 0; i--) {
        double currentPrice = dataArray[i].close;
        double currentPortfolioValue = simCash + (simShares * currentPrice);

        system("clear");
        printf("================= 가상 투자 DAY %d / %d =================\n", (dataCount - i), dataCount);
        printf("날짜: %s\n", dataArray[i].timestamp);
        printf("종가: $%.2f\n", currentPrice);
        // sma5/sma20 -> smaShort/smaLong
        printf("단기 이동평균: $%.2f | 장기 이동평균: $%.2f\n", dataArray[i].smaShort, dataArray[i].smaLong);
        printf("----------------------------------------------------\n");
        printf("현재 자산: $%.2f (현금 $%.2f | 주식 %d주)\n", currentPortfolioValue, simCash, simShares);
        printf("----------------------------------------------------\n");
        printf("무엇을 하시겠습니까?\n");
        printf("[1] 매수   [2] 매도   [3] 하루 넘기기   [0] 종료\n");
        printf("선택: ");

        int choice = getIntegerInput();

        if (choice == 1) {
            printf("몇 주를 매수하시겠습니까?: ");
            int sharesToBuy = getIntegerInput();
            
            if (sharesToBuy > 0 && simCash >= sharesToBuy * currentPrice) {
                simShares += sharesToBuy;
                simCash -= sharesToBuy * currentPrice;
                printf("%d주를 매수했습니다.\n", sharesToBuy);
            } else {
                printf("현금이 부족하거나 잘못된 수량입니다.\n");
            }
            pressEnterToContinue();
        } else if (choice == 2) {
            printf("몇 주를 매도하시겠습니까?: ");
            int sharesToSell = getIntegerInput();

            if (sharesToSell > 0 && simShares >= sharesToSell) {
                simShares -= sharesToSell;
                simCash += sharesToSell * currentPrice;
                printf("%d주를 매도했습니다.\n", sharesToSell);
            } else {
                printf("보유 주식이 부족하거나 잘못된 수량입니다.\n");
            }
            pressEnterToContinue();
        } else if (choice == 0) {
            printf("가상 투자를 조기 종료합니다.\n");
            break;
        }
    }

    double finalValue = simCash + (simShares * dataArray[0].close);
    double initialCash = 10000.0; 
    double totalReturn = ((finalValue - initialCash) / initialCash) * 100;

    printf("\n--- 가상 투자 종료 ---\n");
    printf("초기 자산: $%.2f\n", initialCash);
    printf("최종 자산: $%.2f\n", finalValue);
    printf("총 수익률: %.2f%%\n", totalReturn);
    pressEnterToContinue();
}