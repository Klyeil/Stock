#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataProcessor.h"


int loadCsvFile(const char* filePath, DailyData** dataArray) {
    FILE *fp = fopen(filePath, "r");
    if (!fp) {
        fprintf(stderr, "Error: %s 파일을 열 수 없습니다.\n", filePath);
        return -1;
    }

    char line[1024];
    int dataCount = 0;
    *dataArray = NULL;

    fgets(line, 1024, fp);

    while (fgets(line, 1024, fp)) {
        DailyData* temp = realloc(*dataArray, (dataCount + 1) * sizeof(DailyData));
        if (temp == NULL) {
            fprintf(stderr, "Error: 메모리 재할당에 실패했습니다.\n");
            free(*dataArray);
            fclose(fp);
            return -1;
        }
        *dataArray = temp;

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



void freeData(DailyData* dataArray) {
    free(dataArray);
}


void calculateSma(DailyData* dataArray, int dataCount, int period) {
    if (dataCount < period) {
        return;
    }
    for (int i = dataCount - period; i >= 0; i--) {
        double sum = 0.0;
        for (int j = 0; j < period; j++) {
            sum += dataArray[i + j].close;
        }
        double sma = sum / period;

        if (period == 5) {
            dataArray[i].sma5 = sma;
        } else if (period == 20) {
            dataArray[i].sma20 = sma;
        }
    }
}