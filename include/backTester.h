#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "dataProcessor.h"


typedef struct SimulationState {
    double initialCash; // 초기 자본금
    double cash;        // 현재 현금
    int shares;         // 현재 보유 주식 수
    double portfolioValue; // 포트폴리오 가치
    double totalCost;   // 현재 보유 주식을 매수하는 데 들어간 총 비용
    int totalTrades;    // 총 거래 횟수 (매수 후 매도 시 1회)
    int winningTrades;  // 이익을 본 거래 횟수
    double grossProfit; // 총 수익 금액
    double grossLoss;   // 총 손실 금액
} SimulationState;


void runBacktest(DailyData* dataArray, int dataCount);

#endif