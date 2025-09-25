#include <stdio.h>
#include <string.h>
#include <float.h>
#include "chartPlotter.h"

#define CHART_WIDTH 60
#define CHART_HEIGHT 20

void plotChart(DailyData* dataArray, int dataCount) {
    if (dataCount == 0) return;

    char chart[CHART_HEIGHT][CHART_WIDTH];
    memset(chart, ' ', sizeof(chart));

    int plotCount = (dataCount < CHART_WIDTH) ? dataCount : CHART_WIDTH;
    DailyData* plotData = dataArray;

    double maxPrice = 0.0;
    double minPrice = DBL_MAX;
    for (int i = 0; i < plotCount; i++) {
        if (plotData[i].close > maxPrice) maxPrice = plotData[i].close;
        if (plotData[i].close < minPrice) minPrice = plotData[i].close;
    }

    double priceRange = maxPrice - minPrice;
    if (priceRange < 1e-6) priceRange = 1.0;


    for (int i = 0; i < plotCount; i++) {
        int x = plotCount - 1 - i;
        
        int y = (int)(((plotData[i].close - minPrice) / priceRange) * (CHART_HEIGHT - 1));
        
        chart[CHART_HEIGHT - 1 - y][x] = '*';
    }

    printf("\n--- Price Chart (Last %d days) ---\n\n", plotCount);
    for (int y = 0; y < CHART_HEIGHT; y++) {
        if (y == 0) {
            printf("$%-8.2f |", maxPrice);
        } else if (y == CHART_HEIGHT - 1) {
            printf("$%-8.2f |", minPrice);
        } else {
            printf("%-9s|", "");
        }
        
        for (int x = 0; x < CHART_WIDTH; x++) {
            putchar(chart[y][x]);
        }
        putchar('\n');
    }
    printf("%-9s+------------------------------------------------------------\n", "");
    printf("%10s%s %50s\n", "", plotData[plotCount-1].timestamp, plotData[0].timestamp);
}