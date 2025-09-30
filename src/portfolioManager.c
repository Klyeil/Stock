#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "portfolioManager.h"
#include "apiHandler.h"
#include "mainHelper.h"

void loadPortfolio(Portfolio* portfolio) {
    portfolio->cash = 10000.0;
    strcpy(portfolio->symbol, "NONE");
    portfolio->shares = 0;
    portfolio->avgBuyPrice = 0.0;

    FILE* fp = fopen("data/portfolio.csv", "r");
    if (!fp) {
        printf("portfolio.csv 파일을 찾을 수 없어, 새 포트폴리오를 시작합니다.\n");
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "CASH,%lf", &portfolio->cash);
    }
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "HOLDING,%[^,],%d,%lf", portfolio->symbol, &portfolio->shares, &portfolio->avgBuyPrice);
    }

    fclose(fp);
    printf("기존 포트폴리오를 성공적으로 불러왔습니다.\n");
}

void savePortfolio(const Portfolio* portfolio) {
    FILE* fp = fopen("data/portfolio.csv", "w");
    if (!fp) {
        fprintf(stderr, "에러: portfolio.csv 파일을 저장할 수 없습니다.\n");
        return;
    }

    fprintf(fp, "CASH,%.2f\n", portfolio->cash);
    if (portfolio->shares > 0) {
        fprintf(fp, "HOLDING,%s,%d,%.2f\n", portfolio->symbol, portfolio->shares, portfolio->avgBuyPrice);
    }

    fclose(fp);
    printf("현재 포트폴리오 상태가 저장되었습니다.\n");
}

void managePortfolio(Portfolio* portfolio, const char* apiKey) {
    int choice = -1;
    while(1) {
        system("clear");
        printf("--- 내 모의 포트폴리오 관리 ---\n\n");
        printf("  현재 현금: $%.2f\n", portfolio->cash);
        if (portfolio->shares > 0) {
            printf("  보유 주식: %s %d주 (평단가 $%.2f)\n", portfolio->symbol, portfolio->shares, portfolio->avgBuyPrice);
            
            double currentPrice = fetchCurrentPrice(portfolio->symbol, apiKey);
            if (currentPrice > 0) {
                double currentValue = portfolio->shares * currentPrice;
                double profit = currentValue - (portfolio->shares * portfolio->avgBuyPrice);
                double returnRate = (portfolio->shares > 0 && portfolio->avgBuyPrice > 0) ? (profit / (portfolio->shares * portfolio->avgBuyPrice)) * 100 : 0.0;
                printf("  -----------------------------------\n");
                printf("  실시간 현재가: $%.2f\n", currentPrice);
                printf("  현재 평가액: $%.2f (수익률: %.2f%%, 손익: $%.2f)\n", currentValue, returnRate, profit);
            }
        }
        printf("\n");
        printf("  1. 주식 매수 (기존 주식은 매도 후 가능)\n");
        printf("  2. 주식 매도\n");
        printf("  0. 메인 메뉴로 돌아가기\n");
        printf("----------------------------------\n");
        printf("선택: ");

        scanf("%d", &choice);
        while(getchar() != '\n');

        if (choice == 1) {
            if (portfolio->shares > 0) {
                printf("이미 보유 중인 주식이 있습니다. 매도 후 새로 매수해주세요.\n");
            } else {
                char symbol[16];
                int sharesToBuy;
                printf("매수할 종목 코드: ");
                scanf("%s", symbol);
                while(getchar() != '\n');

                double price = fetchCurrentPrice(symbol, apiKey);
                if (price <= 0) {
                    printf("가격을 조회할 수 없는 종목입니다.\n");
                } else {
                    printf("현재가: $%.2f, 매수할 수량: ", price);
                    scanf("%d", &sharesToBuy);
                    while(getchar() != '\n');
                    
                    if (sharesToBuy > 0 && portfolio->cash >= sharesToBuy * price) {
                        strcpy(portfolio->symbol, symbol);
                        portfolio->shares = sharesToBuy;
                        portfolio->avgBuyPrice = price;
                        portfolio->cash -= sharesToBuy * price;
                        printf("매수 완료.\n");
                    } else {
                        printf("현금이 부족하거나 수량이 잘못되었습니다.\n");
                    }
                }
            }
        } else if (choice == 2) {
            if (portfolio->shares == 0) {
                printf("보유 중인 주식이 없습니다.\n");
            } else {
                printf("보유 중인 %s %d주를 전량 매도합니다.\n", portfolio->symbol, portfolio->shares);
                double price = fetchCurrentPrice(portfolio->symbol, apiKey);
                if (price > 0) {
                    portfolio->cash += portfolio->shares * price;
                    portfolio->shares = 0;
                    strcpy(portfolio->symbol, "NONE");
                    portfolio->avgBuyPrice = 0.0;
                    printf("매도 완료.\n");
                } else {
                    printf("현재가를 조회할 수 없어 매도에 실패했습니다.\n");
                }
            }
        } else if (choice == 0) {
            break;
        }
        pressEnterToContinue();
    }
}