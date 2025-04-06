#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100       // maximum size for stack and input
#define MAX_PROD 10   // maximum number of productions
#define PROD_SIZE 20  // maximum size of a production string

// Function to try reducing the stack using the productions.
// Returns 1 if a reduction occurred, 0 otherwise.
int reduce(char *stack, char productions[][PROD_SIZE], int numProd) {
    int i;
    int reduced = 0;
    int lenStack = strlen(stack);

    // Try each production
    for(i = 0; i < numProd; i++) {
        // A production is assumed to be in the form: A->... 
        char lhs = productions[i][0]; 
        char rhs[PROD_SIZE];
        // Copy the right-hand side from the production (skipping "A->")
        strcpy(rhs, productions[i] + 3);
        int lenRhs = strlen(rhs);

        // Check if the right-hand side is at the end of the stack
        if(lenStack >= lenRhs) {
            if(strcmp(stack + lenStack - lenRhs, rhs) == 0) {
                // Perform the reduction:
                // Remove the matched part and append the LHS symbol.
                stack[lenStack - lenRhs] = '\0';
                char temp[2] = {lhs, '\0'};
                strcat(stack, temp);
                reduced = 1;
                // Update the stack length after reduction.
                lenStack = strlen(stack);
                // Restart checking from the first production.
                i = -1;
            }
        }
    }
    return reduced;
}

int main() {
    int numProd, i, pos = 0;
    char productions[MAX_PROD][PROD_SIZE];
    char input[MAX];
    char stack[MAX] = "";

    printf("Enter the number of productions: ");
    scanf("%d", &numProd);
    
    // Read each production rule.
    // Expected format for each: A->xyz
    for(i = 0; i < numProd; i++) {
        printf("Enter production %d: ", i + 1);
        scanf("%s", productions[i]);
    }

    // The start symbol is assumed to be the LHS of the first production.
    char startSymbol = productions[0][0];

    printf("Enter the input string: ");
    scanf("%s", input);

    int lenInput = strlen(input);

    // Process each character (shift) then try to reduce.
    while(pos < lenInput) {
        // Shift: push the next input symbol onto the stack.
        char shiftSymbol[2] = {input[pos], '\0'};
        pos++;
        strcat(stack, shiftSymbol);
        
        // Try reducing as much as possible.
        while(reduce(stack, productions, numProd));
    }
    
    // After all input is shifted, do one final reduction pass.
    while(reduce(stack, productions, numProd));
    
    // Check if the stack contains only the start symbol.
    if(strlen(stack) == 1 && stack[0] == startSymbol) {
        printf("Input string is Accepted.\n");
    } else {
        printf("Input string is Rejected.\n");
    }
        
    return 0;
}
