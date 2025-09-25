#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H


typedef struct DailyData {
    char timestamp[11]; // "YYYY-MM-DD" + NULL 문자
    double close; // 종가
    long long volume; // 거래량
    double sma5; // 5일 이동평균 저장 공간
    double sma20; // 20일 이동평균 저장 공간
} DailyData;


int loadCsvFile(const char* filePath, DailyData** dataArray);

// 이동평균(SMA) 계산 함수
void calculateSma(DailyData* dataArray, int dataCount, int period);

// 데이터 배열 메모리 해제 함수
void freeData(DailyData* dataArray);

#endif