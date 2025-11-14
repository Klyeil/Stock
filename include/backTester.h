#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "dataProcessor.h"

typedef struct SimulationState {
    double initialCash;
    double cash;
    int shares;
    double portfolioValue;
    double totalCost;
    int totalTrades;
    int winningTrades;
    double grossProfit;
    double grossLoss;
} SimulationState;

SimulationState runBacktest(DailyData* dataArray, int dataCount);

void printDetailedReport(SimulationState simState, DailyData* dataArray, int dataCount, int shortPeriod, int longPeriod);

#endif