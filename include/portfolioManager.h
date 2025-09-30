#ifndef PORTFOLIO_MANAGER_H
#define PORTFOLIO_MANAGER_H

typedef struct Portfolio {
    double cash;
    char symbol[16];
    int shares;
    double avgBuyPrice;
} Portfolio;

void loadPortfolio(Portfolio* portfolio);

void savePortfolio(const Portfolio* portfolio);

void managePortfolio(Portfolio* portfolio, const char* apiKey);

#endif