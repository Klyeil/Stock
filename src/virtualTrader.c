#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "virtualTrader.h"
#include "mainHelper.h"

void startVirtualTrading(DailyData* dataArray, int dataCount, Portfolio* portfolio) {


    if (portfolio->shares > 0 && strcmp(portfolio->symbol, dataArray[0].timestamp) != 0) {
         printf("현재 포트폴리오에 다른 종목(%s)을 보유 중입니다.\n", portfolio->symbol);
         printf("가상 투자는 현재 분석된 종목(%s)을 대상으로만 진행할 수 있습니다.\n", dataArray[0].timestamp);
         pressEnterToContinue();
         return;
    }

    printf("\n--- 가상 투자 시뮬레이션을 시작합니다. ---\n");
    pressEnterToContinue();
    

    for (int i = dataCount - 1; i >= 0; i--) {
        double currentPrice = dataArray[i].close;
        double currentPortfolioValue = portfolio->cash + (portfolio->shares * currentPrice);

        system("clear");
        printf("================= 가상 투자 DAY %d / %d =================\n", (dataCount - i), dataCount);
        printf("날짜: %s\n", dataArray[i].timestamp);
        printf("종가: $%.2f\n", currentPrice);
        printf("5일 이동평균: $%.2f | 20일 이동평균: $%.2f\n", dataArray[i].sma5, dataArray[i].sma20);
        printf("----------------------------------------------------\n");
        printf("현재 자산: $%.2f (현금 $%.2f | 주식 %d주)\n", currentPortfolioValue, portfolio->cash, portfolio->shares);
        printf("----------------------------------------------------\n");
        printf("무엇을 하시겠습니까?\n");
        printf("[1] 매수   [2] 매도   [3] 하루 넘기기   [0] 종료\n");
        printf("선택: ");

        int choice = 0;
        scanf("%d", &choice);
        while(getchar() != '\n');

        if (choice == 1) {
            printf("몇 주를 매수하시겠습니까?: ");
            int sharesToBuy = 0;
            scanf("%d", &sharesToBuy);
            while(getchar() != '\n');
            
            if (sharesToBuy > 0 && portfolio->cash >= sharesToBuy * currentPrice) {
                if (portfolio->shares > 0) {
                    portfolio->avgBuyPrice = ((portfolio->avgBuyPrice * portfolio->shares) + (sharesToBuy * currentPrice)) / (portfolio->shares + sharesToBuy);
                } else {
                    portfolio->avgBuyPrice = currentPrice;
                }

                portfolio->shares += sharesToBuy;
                portfolio->cash -= sharesToBuy * currentPrice;
                strcpy(portfolio->symbol, dataArray[0].timestamp);
                printf("%d주를 매수했습니다.\n", sharesToBuy);

            } else {
                printf("현금이 부족하거나 잘못된 수량입니다.\n");
            }
            pressEnterToContinue();
        } else if (choice == 2) {
            printf("몇 주를 매도하시겠습니까?: ");
            int sharesToSell = 0;
            scanf("%d", &sharesToSell);
            while(getchar() != '\n');

            if (sharesToSell > 0 && portfolio->shares >= sharesToSell) {
                portfolio->shares -= sharesToSell;
                portfolio->cash += sharesToSell * currentPrice;
                printf("%d주를 매도했습니다.\n", sharesToSell);
                if (portfolio->shares == 0) {
                    strcpy(portfolio->symbol, "NONE");
                    portfolio->avgBuyPrice = 0.0;
                }
            } else {
                printf("보유 주식이 부족하거나 잘못된 수량입니다.\n");
            }
            pressEnterToContinue();
        } else if (choice == 0) {
            printf("가상 투자를 조기 종료합니다.\n");
            break;
        }
    }

    double finalValue = portfolio->cash + (portfolio->shares * dataArray[0].close);
    double initialCash = 10000.0; 
    double totalReturn = ((finalValue - initialCash) / initialCash) * 100;

    printf("\n--- 가상 투자 종료 ---\n");
    printf("초기 자산(가정): $%.2f\n", initialCash);
    printf("최종 자산: $%.2f\n", finalValue);
    printf("수익률: %.2f%%\n", totalReturn);
    pressEnterToContinue();
}