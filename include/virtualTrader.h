#ifndef VIRTUAL_TRADER_H
#define VIRTUAL_TRADER_H

#include "dataProcessor.h"
#include "portfolioManager.h"


void startVirtualTrading(DailyData* dataArray, int dataCount, Portfolio* portfolio);

#endif