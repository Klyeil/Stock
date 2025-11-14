#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataProcessor.h"

int loadCsvFile(const char* filePath, DailyData** dataArray) {
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        fprintf(stderr, "에러: %s 파일을 열 수 없습니다.\n", filePath);
        return -1;
    }

    char line[1024];
    int dataCount = 0;
    *dataArray = NULL;

    fgets(line, 1024, fp);

    while (fgets(line, 1024, fp)) {
        DailyData* temp = realloc(*dataArray, (dataCount + 1) * sizeof(DailyData));
        if (temp == NULL) {
            fprintf(stderr, "에러: 메모리 재할당에 실패했습니다.\n");
            free(*dataArray);
            fclose(fp);
            return -1;
        }
        *dataArray = temp;
        
        (*dataArray)[dataCount].smaShort = 0.0;
        (*dataArray)[dataCount].smaLong = 0.0;

        double open, high, low;
        sscanf(line, "%10[^,],%lf,%lf,%lf,%lf,%lld",
               (*dataArray)[dataCount].timestamp,
               &open, &high, &low,
               &(*dataArray)[dataCount].close,
               &(*dataArray)[dataCount].volume);
        
        dataCount++;
    }

    fclose(fp);
    printf("총 %d일 분량의 데이터를 성공적으로 로드했습니다.\n", dataCount);
    return dataCount;
}

void calculateSma(DailyData* dataArray, int dataCount, int period, int isShortTerm) {
    if (dataCount < period) {
        return;
    }
    for (int i = dataCount - period; i >= 0; i--) {
        double sum = 0.0;
        for (int j = 0; j < period; j++) {
            sum += dataArray[i + j].close;
        }
        double sma = sum / period;
        
        if (isShortTerm) {
            dataArray[i].smaShort = sma;
        } else {
            dataArray[i].smaLong = sma;
        }
    }
}

void freeData(DailyData* dataArray) {
    free(dataArray);
}