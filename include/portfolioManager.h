#ifndef PORTFOLIO_MANAGER_H
#define PORTFOLIO_MANAGER_H

typedef struct StockHolding {
    char symbol[16];
    int shares;
    double avgBuyPrice;
} StockHolding;


typedef struct Portfolio {
    double cash;
    StockHolding* holdings;
    int holdingCount;
} Portfolio;


void loadPortfolio(Portfolio* portfolio);

void savePortfolio(const Portfolio* portfolio);

void managePortfolio(Portfolio* portfolio, const char* apiKey);

#endif