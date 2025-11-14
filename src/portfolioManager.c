#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include "portfolioManager.h"
#include "apiHandler.h"
#include "dataProcessor.h"
#include "mainHelper.h"

void loadPortfolio(Portfolio* portfolio) {
    portfolio->cash = 10000.0;
    portfolio->holdings = NULL;
    portfolio->holdingCount = 0;

    FILE* fp = fopen("data/portfolio.csv", "r");
    if (!fp) {
        printf("portfolio.csv 파일을 찾을 수 없어, 새 포트폴리오를 시작합니다. (초기 자본 $10000)\n");
        return;
    }

    char line[256];
    int holdingLines = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "CASH,", 5) == 0) {
            sscanf(line, "CASH,%lf", &portfolio->cash);
        } else if (strncmp(line, "HOLDING,", 8) == 0) {
            holdingLines++;
        }
    }

    portfolio->holdingCount = holdingLines;
    if (portfolio->holdingCount == 0) {
        fclose(fp);
        printf("기존 포트폴리오를 불러왔으나, 보유 종목이 없습니다.\n");
        return; 
    }

    portfolio->holdings = malloc(portfolio->holdingCount * sizeof(StockHolding));
    if (portfolio->holdings == NULL) {
        fprintf(stderr, "에러: 포트폴리오 메모리 할당 실패\n");
        fclose(fp);
        return;
    }

    rewind(fp); 

    int currentIndex = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "HOLDING,", 8) == 0) {
            if (currentIndex >= portfolio->holdingCount) {
                break; 
            }
            StockHolding* currentHolding = &portfolio->holdings[currentIndex];
            sscanf(line, "HOLDING,%[^,],%d,%lf", currentHolding->symbol, &currentHolding->shares, &currentHolding->avgBuyPrice);
            currentIndex++;
        }
    }

    fclose(fp);
    printf("기존 포트폴리오를 성공적으로 불러왔습니다. (보유 종목: %d개)\n", portfolio->holdingCount);
}

void savePortfolio(const Portfolio* portfolio) {
    FILE* fp = fopen("data/portfolio.csv", "w");
    if (!fp) {
        fprintf(stderr, "에러: data/portfolio.csv 파일을 저장할 수 없습니다.\n");
        return;
    }

    fprintf(fp, "CASH,%.2f\n", portfolio->cash);
    for (int i = 0; i < portfolio->holdingCount; i++) {
        fprintf(fp, "HOLDING,%s,%d,%.2f\n", 
                portfolio->holdings[i].symbol, 
                portfolio->holdings[i].shares, 
                portfolio->holdings[i].avgBuyPrice);
    }

    fclose(fp);
    printf("현재 포트폴리오 상태가 저장되었습니다.\n");
}

void managePortfolio(Portfolio* portfolio, const char* apiKey) {
    int choice = -1;
    while(1) {
        char dateStr[20];
        time_t now = time(NULL);
        struct tm *localTime = localtime(&now);
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localTime);

        system("clear");
        printf("--- 내 모의 포트폴리오 관리 (%s 기준) ---\n\n", dateStr);
        printf("  현재 현금: $%.2f\n\n", portfolio->cash);
        
        double totalEquity = portfolio->cash;

        double* currentPrices = NULL;
        if (portfolio->holdingCount > 0) {
            currentPrices = malloc(portfolio->holdingCount * sizeof(double));
            if (currentPrices == NULL) {
                fprintf(stderr, "포트폴리오 가격 배열 메모리 할당 실패\n");
                return; 
            }

            printf("보유 종목 데이터 확인 및 로드 중...\n");
            for (int i = 0; i < portfolio->holdingCount; i++) {
                StockHolding* holding = &portfolio->holdings[i];
                
                int fetchResult = fetchAndSaveData(holding->symbol, apiKey);
                
                if (fetchResult < 0) {
                    printf("  %s 데이터 가져오기 실패...\n", holding->symbol);
                    currentPrices[i] = -1.0;
                    continue;
                }

                char filePath[256];
                snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", holding->symbol);

                DailyData* stockData = NULL;
                int dataCount = loadCsvFile(filePath, &stockData);

                if (dataCount > 0) {
                    currentPrices[i] = stockData[0].close;
                    printf("  %s 로드 완료 (최근 종가: $%.2f)\n", holding->symbol, currentPrices[i]);
                    freeData(stockData);
                } else {
                    printf("  %s 데이터 로드 실패 (파일은 있으나 데이터 없음)\n", holding->symbol);
                    currentPrices[i] = -1.0;
                }
                if (fetchResult == 1 && i < portfolio->holdingCount - 1) {
                    printf("  (신규 API 호출됨. 15초 대기...)\n");
                    sleep(15);
                }
            }
            printf("조회 완료.\n\n");
        }
        // ------------------

        system("clear");
        printf("--- 내 모의 포트폴리오 관리 (%s 기준) ---\n\n", dateStr);
        printf("  현재 현금: $%.2f\n\n", portfolio->cash);

        if (portfolio->holdingCount == 0) {
            printf("  보유 중인 주식이 없습니다.\n");
        } else {
            printf("  [보유 주식 목록]\n");
            for (int i = 0; i < portfolio->holdingCount; i++) {
                StockHolding* holding = &portfolio->holdings[i];
                printf("  %d. %s: %d주 (평단가 $%.2f)\n", i + 1, holding->symbol, holding->shares, holding->avgBuyPrice);
                
                double currentPrice = currentPrices[i]; 

                if (currentPrice > 0) {
                    double currentValue = holding->shares * currentPrice;
                    double profit = currentValue - (holding->shares * holding->avgBuyPrice);
                    double returnRate = (holding->avgBuyPrice > 0) ? (profit / (holding->shares * holding->avgBuyPrice)) * 100 : 0.0;
                    printf("     -> 현재가(종가): $%.2f | 평가액: $%.2f (손익: $%.2f, %.2f%%)\n", 
                           currentPrice, currentValue, profit, returnRate);
                    totalEquity += currentValue;
                } else {
                    printf("     -> 현재가: (조회 실패) | 평가액: $0.00 (손익: $0.00, 0.00%%)\n");
                }
            }
        }
        
        printf("\n  ----------------------------------\n");
        printf("  총 자산 평가액: $%.2f\n", totalEquity);
        printf("  ----------------------------------\n");
        printf("  1. 주식 매수\n");
        printf("  2. 주식 매도\n");
        printf("  0. 메인 메뉴로 돌아가기\n");
        printf("  ----------------------------------\n");
        printf("선택: ");

        choice = getIntegerInput();

        if (choice == 1) { 
            char symbol[16];
            int sharesToBuy;
            printf("매수할 종목 코드: ");
            getStringInput(symbol, sizeof(symbol));

            double price = -1.0;
            int foundIndex = -1;
            for (int i = 0; i < portfolio->holdingCount; i++) {
                if (strcmp(portfolio->holdings[i].symbol, symbol) == 0) {
                    price = currentPrices[i]; 
                    foundIndex = i;
                    break;
                }
            }

            if (price <= 0) { 
                printf("신규 종목 API 조회... ");
                int fetchResult = fetchAndSaveData(symbol, apiKey);
                if (fetchResult >= 0) {
                    char filePath[256];
                    snprintf(filePath, sizeof(filePath), "data/%s_daily.csv", symbol);
                    DailyData* stockData = NULL;
                    int dataCount = loadCsvFile(filePath, &stockData);
                    if (dataCount > 0) {
                        price = stockData[0].close;
                        printf(" (최근 종가 $%.2f)\n", price);
                        freeData(stockData);
                    } else {
                        printf(" (데이터 로드 실패)\n");
                    }
                } else {
                     printf(" (API 조회 실패)\n");
                }
            }

            if (price <= 0) {
                printf("가격을 조회할 수 없는 종목입니다.\n");
            } else {
                printf("현재가(종가): $%.2f, 매수할 수량: ", price);
                sharesToBuy = getIntegerInput();
                
                if (sharesToBuy <= 0 || portfolio->cash < sharesToBuy * price) {
                    printf("현금이 부족하거나 수량이 잘못되었습니다.\n");
                } else {
                    if (foundIndex != -1) {
                        StockHolding* holding = &portfolio->holdings[foundIndex];
                        holding->avgBuyPrice = ((holding->avgBuyPrice * holding->shares) + (sharesToBuy * price)) / (holding->shares + sharesToBuy);
                        holding->shares += sharesToBuy;
                    } else {
                        portfolio->holdingCount++;
                        portfolio->holdings = realloc(portfolio->holdings, portfolio->holdingCount * sizeof(StockHolding));
                        StockHolding* newHolding = &portfolio->holdings[portfolio->holdingCount - 1];
                        strcpy(newHolding->symbol, symbol);
                        newHolding->shares = sharesToBuy;
                        newHolding->avgBuyPrice = price;
                    }
                    portfolio->cash -= sharesToBuy * price;
                    printf("매수 완료.\n");
                }
            }
        } else if (choice == 2) {
            char symbol[16];
            int sharesToSell;
            printf("매도할 종목 코드: ");
            getStringInput(symbol, sizeof(symbol));

            double price = -1.0;
            int foundIndex = -1;
            for (int i = 0; i < portfolio->holdingCount; i++) {
                if (strcmp(portfolio->holdings[i].symbol, symbol) == 0) {
                    price = currentPrices[i]; 
                    foundIndex = i;
                    break;
                }
            }

            if (foundIndex == -1) {
                printf("보유하지 않은 종목입니다.\n");
            } else {
                StockHolding* holding = &portfolio->holdings[foundIndex];
                printf("현재 보유 수량: %d주, 매도할 수량 (전량: %d): ", holding->shares, holding->shares);
                sharesToSell = getIntegerInput();

                if (sharesToSell <= 0 || sharesToSell > holding->shares) {
                    printf("매도 수량이 잘못되었습니다.\n");
                } else {
                    if (price <= 0) {
                        printf("현재가를 조회할 수 없어 매도에 실패했습니다. (API 조회 실패)\n");
                    } else {
                        portfolio->cash += sharesToSell * price;
                        holding->shares -= sharesToSell;
                        
                        if (holding->shares == 0) {
                            portfolio->holdings[foundIndex] = portfolio->holdings[portfolio->holdingCount - 1];
                            portfolio->holdingCount--;
                            if (portfolio->holdingCount == 0) {
                                free(portfolio->holdings);
                                portfolio->holdings = NULL;
                            } else {
                                portfolio->holdings = realloc(portfolio->holdings, portfolio->holdingCount * sizeof(StockHolding));
                            }
                        }
                        printf("매도 완료.\n");
                    }
                }
            }
        } else if (choice == 0) {
            if (currentPrices != NULL) {
                free(currentPrices);
            }
            break;
        }

        if (currentPrices != NULL) {
            free(currentPrices);
        }
        pressEnterToContinue();
    }
}