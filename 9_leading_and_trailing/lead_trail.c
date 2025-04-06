#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 10  // Max number of non-terminals
#define PROD_SIZE 20 // Max size of each production

// Structure to store leading and trailing sets
typedef struct {
    char nonTerminal;
    char leading[MAX];
    char trailing[MAX];
    int leadCount;
    int trailCount;
} SymbolSet;

SymbolSet symbols[MAX]; // Stores leading and trailing sets
int numProductions;

// Function to add a symbol to the set if not already present
void addSymbol(char *set, int *count, char symbol) {
    for (int i = 0; i < *count; i++) {
        if (set[i] == symbol) return;  // Avoid duplicates
    }
    set[(*count)++] = symbol;
}

// Function to find index of a non-terminal in symbols array
int findIndex(char nonTerm, int numSymbols) {
    for (int i = 0; i < numSymbols; i++) {
        if (symbols[i].nonTerminal == nonTerm) return i;
    }
    return -1;
}

// Function to compute leading symbols (with proper recursion)
void computeLeading(char productions[MAX][PROD_SIZE], int numSymbols) {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < numSymbols; i++) {
            char nonTerm = symbols[i].nonTerminal;
            
            for (int j = 0; j < numProductions; j++) {
                if (productions[j][0] == nonTerm) {
                    char *rhs = productions[j] + 3; // Skip "A->"

                    if (islower(rhs[0]) || ispunct(rhs[0])) { // Terminal
                        if (strchr(symbols[i].leading, rhs[0]) == NULL) {
                            addSymbol(symbols[i].leading, &symbols[i].leadCount, rhs[0]);
                            changed = 1;
                        }
                    } else { // Non-terminal
                        int idx = findIndex(rhs[0], numSymbols);
                        if (idx != -1) {
                            for (int l = 0; l < symbols[idx].leadCount; l++) {
                                if (strchr(symbols[i].leading, symbols[idx].leading[l]) == NULL) {
                                    addSymbol(symbols[i].leading, &symbols[i].leadCount, symbols[idx].leading[l]);
                                    changed = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Function to compute trailing symbols (with proper recursion)
void computeTrailing(char productions[MAX][PROD_SIZE], int numSymbols) {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < numSymbols; i++) {
            char nonTerm = symbols[i].nonTerminal;
            
            for (int j = 0; j < numProductions; j++) {
                if (productions[j][0] == nonTerm) {
                    char *rhs = productions[j] + 3; // Skip "A->"
                    int len = strlen(rhs);
                    char last = rhs[len - 1];

                    if (islower(last) || ispunct(last)) { // Terminal
                        if (strchr(symbols[i].trailing, last) == NULL) {
                            addSymbol(symbols[i].trailing, &symbols[i].trailCount, last);
                            changed = 1;
                        }
                    } else { // Non-terminal
                        int idx = findIndex(last, numSymbols);
                        if (idx != -1) {
                            for (int l = 0; l < symbols[idx].trailCount; l++) {
                                if (strchr(symbols[i].trailing, symbols[idx].trailing[l]) == NULL) {
                                    addSymbol(symbols[i].trailing, &symbols[i].trailCount, symbols[idx].trailing[l]);
                                    changed = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    int numSymbols;
    char productions[MAX][PROD_SIZE];

    // Get number of productions
    printf("Enter the number of productions: ");
    scanf("%d", &numProductions);

    // Read productions
    printf("Enter the productions (Format: A->xyz):\n");
    for (int i = 0; i < numProductions; i++) {
        scanf("%s", productions[i]);
    }

    // Get unique non-terminals
    numSymbols = 0;
    for (int i = 0; i < numProductions; i++) {
        char lhs = productions[i][0];
        if (findIndex(lhs, numSymbols) == -1) { // Check if already added
            symbols[numSymbols].nonTerminal = lhs;
            symbols[numSymbols].leadCount = 0;
            symbols[numSymbols].trailCount = 0;
            numSymbols++;
        }
    }

    // Compute leading and trailing sets
    computeLeading(productions, numSymbols);
    computeTrailing(productions, numSymbols);

    // Display results
    printf("\nLEADING and TRAILING sets:\n");
    for (int i = 0; i < numSymbols; i++) {
        printf("\nLEADING(%c) = { ", symbols[i].nonTerminal);
        for (int j = 0; j < symbols[i].leadCount; j++) {
            printf("%c ", symbols[i].leading[j]);
        }
        printf("}");

        printf("\nTRAILING(%c) = { ", symbols[i].nonTerminal);
        for (int j = 0; j < symbols[i].trailCount; j++) {
            printf("%c ", symbols[i].trailing[j]);
        }
        printf("}\n");
    }

    return 0;
}

