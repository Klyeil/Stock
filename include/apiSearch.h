#ifndef API_SEARCH_H
#define API_SEARCH_H

// typedef struct SearchResult {
//     char symbol[16];
//     char name[128];
// } SearchResult;

int searchSymbols(const char* keywords, const char* apiKey);

#endif