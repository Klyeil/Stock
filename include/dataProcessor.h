#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H


typedef struct DailyData {
    char timestamp[11];
    double close;
    long long volume;
    double sma5;
    double sma20;
} DailyData;


int loadCsvFile(const char* filePath, DailyData** dataArray);

void calculateSma(DailyData* dataArray, int dataCount, int period);

void freeData(DailyData* dataArray);

#endif