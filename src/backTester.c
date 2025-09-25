#include <stdio.h>
#include <math.h>
#include "backTester.h"

void runBacktest(DailyData* dataArray, int dataCount) {
    if (dataCount < 21) {
        printf("백테스트를 실행하기에 데이터가 부족합니다.\n");
        return;
    }

    SimulationState simState;
    simState.initialCash = 10000.0;
    simState.cash = simState.initialCash;
    simState.shares = 0;
    simState.portfolioValue = simState.initialCash;
    simState.totalCost = 0.0;
    simState.totalTrades = 0;
    simState.winningTrades = 0;
    simState.grossProfit = 0.0;
    simState.grossLoss = 0.0;

    printf("\n--- 백테스트 실행 ---\n");
    printf("초기 자본금: $%.2f\n", simState.initialCash);

    for (int i = dataCount - 2; i >= 0; i--) {
        double prevSma5 = dataArray[i + 1].sma5;
        double prevSma20 = dataArray[i + 1].sma20;
        double currentSma5 = dataArray[i].sma5;
        double currentSma20 = dataArray[i].sma20;
        double currentPrice = dataArray[i].close;

        if (prevSma5 < prevSma20 && currentSma5 > currentSma20 && simState.shares == 0) {
            int sharesToBuy = simState.cash / currentPrice;
            if (sharesToBuy > 0) {
                simState.shares = sharesToBuy;
                simState.cash -= sharesToBuy * currentPrice;
                simState.totalCost = sharesToBuy * currentPrice;
                printf("[%s] 매수: %d 주 @ $%.2f\n", dataArray[i].timestamp, sharesToBuy, currentPrice);
            }
        }
        else if (prevSma5 > prevSma20 && currentSma5 < currentSma20 && simState.shares > 0) {
            double saleValue = simState.shares * currentPrice;
            double profit = saleValue - simState.totalCost;
            
            simState.cash += saleValue;
            printf("[%s] 매도: %d 주 @ $%.2f | 손익: $%.2f\n", dataArray[i].timestamp, simState.shares, currentPrice, profit);
            simState.shares = 0;

            simState.totalTrades++;
            if (profit > 0) {
                simState.winningTrades++;
                simState.grossProfit += profit;
            } else {
                simState.grossLoss += profit;
            }
        }
    }

    // --- 최종 리포트 계산 및 출력 (수정된 로직) ---
    double finalPrice = dataArray[0].close;
    double finalEquity = simState.cash + (simState.shares * finalPrice); // 최종 자산 = 현금 + 보유주식 평가액
    double totalReturn = ((finalEquity - simState.initialCash) / simState.initialCash) * 100;
    double winRate = simState.totalTrades > 0 ? ((double)simState.winningTrades / simState.totalTrades) * 100 : 0.0;
    double profitFactor = simState.grossLoss != 0 ? fabs(simState.grossProfit / simState.grossLoss) : 0.0;

    printf("\n--- 백테스트 종료: 최종 리포트 ---\n\n");
    printf("분석 기간: \t\t%s ~ %s\n", dataArray[dataCount - 1].timestamp, dataArray[0].timestamp);
    printf("--------------------------------------------\n");
    printf("초기 자산: \t\t$%.2f\n", simState.initialCash);
    printf("최종 자산: \t\t$%.2f\n", finalEquity);
    printf("총 수익률: \t\t%.2f%%\n", totalReturn);
    
    printf("\n--- 거래 통계 (확정 손익 기준) ---\n");
    printf("총 거래 횟수: \t\t%d회\n", simState.totalTrades);
    printf("승률: \t\t\t%.2f%%\n", winRate);
    printf("수익비 (Profit Factor): \t%.2f\n", profitFactor);
    
    // --- 보유 중인 포지션에 대한 정보 추가 ---
    if (simState.shares > 0) {
        double currentPositionValue = simState.shares * finalPrice;
        double unrealizedProfit = currentPositionValue - simState.totalCost;
        printf("\n--- 현재 보유 포지션 (미실현 손익) ---\n");
        printf("보유 주식: \t\t%d 주\n", simState.shares);
        printf("현재 평가액: \t\t$%.2f\n", currentPositionValue);
        printf("미실현 손익: \t\t$%.2f\n", unrealizedProfit);
    }
    printf("--------------------------------------------\n");
}