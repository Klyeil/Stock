#include <stdio.h>
#include <math.h>
#include "backTester.h"

void printDetailedReport(SimulationState simState, DailyData* dataArray, int dataCount, int shortPeriod, int longPeriod) {
    double finalPrice = dataArray[0].close;
    double finalEquity = simState.cash + (simState.shares * finalPrice);
    double totalReturn = ((finalEquity - simState.initialCash) / simState.initialCash) * 100;
    double winRate = simState.totalTrades > 0 ? ((double)simState.winningTrades / simState.totalTrades) * 100 : 0.0;
    double profitFactor = simState.grossLoss != 0 ? fabs(simState.grossProfit / simState.grossLoss) : 0.0;

    printf("\n--- 백테스트 종료: 최종 리포트 (상세 해설) ---\n\n");
    
    printf("[분석 요약]\n");
    printf("분석 기간(%s ~ %s) 동안,\n", dataArray[dataCount - 1].timestamp, dataArray[0].timestamp);
    printf("분석 전략: \t\t%d일(단기) / %d일(장기) 이동평균 교차\n", shortPeriod, longPeriod);
    printf("초기 자본 $%.2f은(는) 최종 자산 $%.2f(으)로 마감되었습니다.\n\n", simState.initialCash, finalEquity);

    printf("[성과 지표 해설]\n");
    printf("1. 총 수익률: %.2f%%\n", totalReturn);
    printf("   => 초기 자본 대비 자산이 얼마나 늘었는지 보여줍니다.\n");
    if (totalReturn > 0) {
        printf("   => 해석: 플러스(+) 수익을 기록했습니다. \n\n");
    } else {
        printf("   => 해석: 마이너스(-) 손실을 기록했습니다. \n\n");
    }

    printf("2. 총 거래 횟수: %d회\n", simState.totalTrades);
    printf("   => '매수 후 매도'가 완료된 거래의 총 횟수입니다.\n\n");

    printf("3. 승률: %.2f%%\n", winRate);
    printf("   => 전체 거래 중 이익을 본 거래의 비율입니다.\n");
    if (simState.totalTrades > 0) {
        if (winRate > 50.0) {
            printf("   => 해석: 이긴 거래가 진 거래보다 많았습니다.\n\n");
        } else if (winRate == 50.0) {
            printf("   => 해석: 이긴 거래와 진 거래의 횟수가 같습니다.\n\n");
        } else {
            printf("   => 해석: 진 거래가 이긴 거래보다 많았습니다.\n\n");
        }
    }

    printf("4. 수익비 (Profit Factor): %.2f\n", profitFactor);
    printf("   => 총 수익을 총 손실로 나눈 값으로, 전략의 수익성을 판단합니다.\n");
    if (simState.grossLoss == 0 && simState.grossProfit > 0) {
        printf("   => 해석: 손실을 본 거래가 한 번도 없었습니다. (이상적인 결과)\n\n");
    } else if (profitFactor > 1.0) {
        printf("   => 해석: 총 수익이 총 손실보다 %.2f배 많았습니다. (수익성 있는 전략)\n\n", profitFactor);
    } else {
        printf("   => 해석: 총 손실이 총 수익보다 많거나 같았습니다. (개선이 필요한 전략)\n\n");
    }

    if (simState.shares > 0) {
        double currentPositionValue = simState.shares * finalPrice;
        double unrealizedProfit = currentPositionValue - simState.totalCost;
        printf("[현재 보유 포지션 (미실현 손익)]\n");
        printf("보유 주식: %d 주, 현재 평가액: $%.2f, 미실현 손익: $%.2f\n",
               simState.shares, currentPositionValue, unrealizedProfit);
    }
     printf("--------------------------------------------------\n");
}


SimulationState runBacktest(DailyData* dataArray, int dataCount) {
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

    for (int i = dataCount - 2; i >= 0; i--) {
        double prevSmaShort = dataArray[i + 1].smaShort;
        double prevSmaLong = dataArray[i + 1].smaLong;
        double currentSmaShort = dataArray[i].smaShort;
        double currentSmaLong = dataArray[i].smaLong;
        double currentPrice = dataArray[i].close;
        

        if (prevSmaShort == 0.0 || prevSmaLong == 0.0 || currentSmaShort == 0.0 || currentSmaLong == 0.0) {
            continue;
        }

        if (prevSmaShort < prevSmaLong && currentSmaShort > currentSmaLong && simState.shares == 0) {
            int sharesToBuy = simState.cash / currentPrice;
            if (sharesToBuy > 0) {
                simState.shares = sharesToBuy;
                simState.cash -= sharesToBuy * currentPrice;
                simState.totalCost = sharesToBuy * currentPrice;
            }
        }
        else if (prevSmaShort > prevSmaLong && currentSmaShort < currentSmaLong && simState.shares > 0) {
            double saleValue = simState.shares * currentPrice;
            double profit = saleValue - simState.totalCost;
            
            simState.cash += saleValue;
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
    return simState;
}