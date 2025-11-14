#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

typedef struct DailyData {
    char timestamp[11];
    double close;
    long long volume;
    double smaShort;
    double smaLong;
} DailyData;


int loadCsvFile(const char* filePath, DailyData** dataArray);

void calculateSma(DailyData* dataArray, int dataCount, int period, int isShortTerm);

void freeData(DailyData* dataArray);

#endif