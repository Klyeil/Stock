#include <stdio.h>
#include "apiHandler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <stockSymbol>\n", argv[0]);
        return 1;
    }
    
    const char* symbol = argv[1];
    const char* apiKey = "W46WANT3781FTIOS";

    printf("Starting stock analysis for %s...\n", symbol);
    
    fetchAndSaveData(symbol, apiKey);
    
    return 0;
}