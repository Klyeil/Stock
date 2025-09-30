#include <stdio.h>
#include "mainHelper.h"

void pressEnterToContinue() {
    printf("\n계속하려면 Enter 키를 누르세요...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    getchar();
}