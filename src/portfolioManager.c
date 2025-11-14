#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "portfolioManager.h"
#include "apiHandler.h"
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
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "CASH,%lf", &portfolio->cash);
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "HOLDING,", 8) == 0) {
            portfolio->holdingCount++;
            portfolio->holdings = realloc(portfolio->holdings, portfolio->holdingCount * sizeof(StockHolding));
            
            StockHolding* newHolding = &portfolio->holdings[portfolio->holdingCount - 1];
            sscanf(line, "HOLDING,%[^,],%d,%lf", newHolding->symbol, &newHolding->shares, &newHolding->avgBuyPrice);
        }
    }

    fclose(fp);
    printf("기존 포트폴리오를 성공적으로 불러왔습니다. (보유 종목: %d개)\n", portfolio->holdingCount);
}

void savePortfolio(const Portfolio* portfolio) {
    FILE* fp = fopen("portfolio.csv", "w");
    if (!fp) {
        fprintf(stderr, "에러: portfolio.csv 파일을 저장할 수 없습니다.\n");
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

        if (portfolio->holdingCount == 0) {
            printf("  보유 중인 주식이 없습니다.\n");
        } else {
            printf("  [보유 주식 목록]\n");
            for (int i = 0; i < portfolio->holdingCount; i++) {
                StockHolding* holding = &portfolio->holdings[i];
                printf("  %d. %s: %d주 (평단가 $%.2f)\n", i + 1, holding->symbol, holding->shares, holding->avgBuyPrice);
                
                double currentPrice = fetchCurrentPrice(holding->symbol, apiKey);
                if (currentPrice > 0) {
                    double currentValue = holding->shares * currentPrice;
                    double profit = currentValue - (holding->shares * holding->avgBuyPrice);
                    double returnRate = (holding->avgBuyPrice > 0) ? (profit / (holding->shares * holding->avgBuyPrice)) * 100 : 0.0;
                    printf("     -> 현재가: $%.2f | 평가액: $%.2f (손익: $%.2f, %.2f%%)\n", 
                           currentPrice, currentValue, profit, returnRate);
                    totalEquity += currentValue;
                }
            }
        }
        
        printf("\n  ----------------------------------\n");
        printf("  총 자산 평가액: $%.2f\n", totalEquity);
        printf("  ----------------------------------\n");
        printf("  1. 주식 매수\n");
        printf("  2. 주식 매도\n");
        printf("  0. 메인 메뉴로 돌아가기\n");
        printf("----------------------------------\n");
        printf("선택: ");

        choice = getIntegerInput();

        if (choice == 1) { // 매수
            char symbol[16];
            int sharesToBuy;
            printf("매수할 종목 코드: ");
            getStringInput(symbol, sizeof(symbol));

            double price = fetchCurrentPrice(symbol, apiKey);
            if (price <= 0) {
                printf("가격을 조회할 수 없는 종목입니다.\n");
            } else {
                printf("현재가: $%.2f, 매수할 수량: ", price);
                sharesToBuy = getIntegerInput();
                
                if (sharesToBuy <= 0 || portfolio->cash < sharesToBuy * price) {
                    printf("현금이 부족하거나 수량이 잘못되었습니다.\n");
                } else {
                    int foundIndex = -1;
                    for (int i = 0; i < portfolio->holdingCount; i++) {
                        if (strcmp(portfolio->holdings[i].symbol, symbol) == 0) {
                            foundIndex = i;
                            break;
                        }
                    }

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
        } else if (choice == 2) { // 매도
            char symbol[16];
            int sharesToSell;
            printf("매도할 종목 코드: ");
            getStringInput(symbol, sizeof(symbol));

            int foundIndex = -1;
            for (int i = 0; i < portfolio->holdingCount; i++) {
                if (strcmp(portfolio->holdings[i].symbol, symbol) == 0) {
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
                    double price = fetchCurrentPrice(symbol, apiKey);
                    if (price <= 0) {
                        printf("현재가를 조회할 수 없어 매도에 실패했습니다.\n");
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
            break;
        }
        pressEnterToContinue();
    }
}