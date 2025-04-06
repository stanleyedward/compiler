#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_GRAMMAR_SIZE 20
#define MAX_SYMBOL_LENGTH 10

// Grammar representation using simple arrays
typedef struct {
    char lhs;
    char rhs[20];
    int length;
} Production;

Production grammar[MAX_GRAMMAR_SIZE];
int num_productions = 0;
char start_symbol;

// Sets for FIRST and FOLLOW
char first[26][MAX_GRAMMAR_SIZE];  // Array for each uppercase letter (A-Z)
char follow[26][MAX_GRAMMAR_SIZE]; 
int first_size[26] = {0};
int follow_size[26] = {0};
bool first_computed[26] = {false};
bool follow_computed[26] = {false};

// Add production to the grammar
void add_production(char lhs, const char* rhs) {
    grammar[num_productions].lhs = lhs;
    grammar[num_productions].length = strlen(rhs);
    strcpy(grammar[num_productions].rhs, rhs);
    num_productions++;
}

// Check if a character c is in set
bool is_in_set(char c, char* set, int set_size) {
    for (int i = 0; i < set_size; i++) {
        if (set[i] == c)
            return true;
    }
    return false;
}

// Add c to set if not already present
void add_to_set(char c, char* set, int* set_size) {
    if (!is_in_set(c, set, *set_size)) {
        set[(*set_size)++] = c;
    }
}

// Add all elements from source_set to dest_set
void union_sets(char* source_set, int source_size, char* dest_set, int* dest_size) {
    for (int i = 0; i < source_size; i++) {
        add_to_set(source_set[i], dest_set, dest_size);
    }
}

// Check if epsilon is in the first set of X
bool has_epsilon(char X) {
    int idx = X - 'A';
    return is_in_set('e', first[idx], first_size[idx]);
}

// Print sets
void print_sets() {
    printf("FIRST SETS:\n");
    for (int i = 0; i < 26; i++) {
        if (first_size[i] > 0) {
            printf("FIRST(%c) = { ", 'A' + i);
            for (int j = 0; j < first_size[i]; j++) {
                printf("%c", first[i][j]);
                if (j < first_size[i] - 1)
                    printf(", ");
            }
            printf(" }\n");
        }
    }
    
    printf("\nFOLLOW SETS:\n");
    for (int i = 0; i < 26; i++) {
        if (follow_size[i] > 0) {
            printf("FOLLOW(%c) = { ", 'A' + i);
            for (int j = 0; j < follow_size[i]; j++) {
                printf("%c", follow[i][j]);
                if (j < follow_size[i] - 1)
                    printf(", ");
            }
            printf(" }\n");
        }
    }
}

int main() {
    // Simple grammar:
    // S -> A B
    // A -> a | e
    // B -> b
    add_production('S', "AB");
    add_production('A', "a");
    add_production('A', "e");  // e represents epsilon
    add_production('B', "b");
    
    start_symbol = 'S';
    
    // Calculate FIRST for all non-terminals
    for (int i = 0; i < num_productions; i++) {
        char nt = grammar[i].lhs;
        if (!first_computed[nt - 'A'])
            compute_first(nt);
    }
    
    // Calculate FOLLOW for all non-terminals
    for (int i = 0; i < num_productions; i++) {
        char nt = grammar[i].lhs;
        if (!follow_computed[nt - 'A'])
            compute_follow(nt);
    }
    
    // Print results
    print_sets();
    
    return 0;
}

// Calculate FIRST set for a non-terminal
void compute_first(char X) {
    int idx = X - 'A';
    
    // If already calculated
    if (first_computed[idx])
        return;
    
    first_computed[idx] = true;
    
    // Go through all productions where X is on the LHS
    for (int i = 0; i < num_productions; i++) {
        if (grammar[i].lhs == X) {
            // Case 1: X -> ε
            if (grammar[i].length == 1 && grammar[i].rhs[0] == 'e') {
                add_to_set('e', first[idx], &first_size[idx]);
            } 
            // Case 2: X -> Y...
            else {
                bool added_epsilon = true;
                
                // Process each symbol in the right-hand side
                for (int j = 0; j < grammar[i].length; j++) {
                    char Y = grammar[i].rhs[j];
                    
                    // If terminal, add to FIRST(X) and break
                    if (islower(Y) || Y == '(' || Y == ')' || Y == '$' || Y == '+' || Y == '*') {
                        add_to_set(Y, first[idx], &first_size[idx]);
                        added_epsilon = false;
                        break;
                    }
                    // If non-terminal, compute FIRST(Y) and add to FIRST(X)
                    else if (isupper(Y)) {
                        compute_first(Y);
                        int Y_idx = Y - 'A';
                        
                        // Add all except epsilon from FIRST(Y) to FIRST(X)
                        for (int k = 0; k < first_size[Y_idx]; k++) {
                            if (first[Y_idx][k] != 'e') {
                                add_to_set(first[Y_idx][k], first[idx], &first_size[idx]);
                            }
                        }
                        
                        // If no epsilon in FIRST(Y), break
                        if (!has_epsilon(Y)) {
                            added_epsilon = false;
                            break;
                        }
                    }
                }
                
                // If we went through all symbols and they all have epsilon,
                // add epsilon to FIRST(X)
                if (added_epsilon) {
                    add_to_set('e', first[idx], &first_size[idx]);
                }
            }
        }
    }
}

// Calculate FOLLOW set for a non-terminal
void compute_follow(char X) {
    int idx = X - 'A';
    
    // If already calculated
    if (follow_computed[idx])
        return;
    
    follow_computed[idx] = true;
    
    // For start symbol, add $ to FOLLOW set
    if (X == start_symbol) {
        add_to_set('$', follow[idx], &follow_size[idx]);
    }
    
    // For each production
    for (int i = 0; i < num_productions; i++) {
        // Find X in RHS of the production
        for (int j = 0; j < grammar[i].length; j++) {
            if (grammar[i].rhs[j] == X) {
                // Case 1: A -> αXβ, add FIRST(β) to FOLLOW(X)
                if (j < grammar[i].length - 1) {
                    char beta = grammar[i].rhs[j+1];
                    
                    if (islower(beta) || beta == '(' || beta == ')' || beta == '$' || beta == '+' || beta == '*') {
                        add_to_set(beta, follow[idx], &follow_size[idx]);
                    } else if (isupper(beta)) {
                        int beta_idx = beta - 'A';
                        if (!first_computed[beta_idx])
                            compute_first(beta);
                            
                        // Add all except epsilon
                        for (int k = 0; k < first_size[beta_idx]; k++) {
                            if (first[beta_idx][k] != 'e') {
                                add_to_set(first[beta_idx][k], follow[idx], &follow_size[idx]);
                            }
                        }
                        
                        // If epsilon in FIRST(β), add FOLLOW(A) to FOLLOW(X)
                        if (has_epsilon(beta)) {
                            compute_follow(grammar[i].lhs);
                            union_sets(follow[grammar[i].lhs - 'A'], 
                                      follow_size[grammar[i].lhs - 'A'], 
                                      follow[idx], 
                                      &follow_size[idx]);
                        }
                    }
                }
                // Case 2: A -> αX, add FOLLOW(A) to FOLLOW(X)
                else if (j == grammar[i].length - 1 && grammar[i].lhs != X) {
                    compute_follow(grammar[i].lhs);
                    union_sets(follow[grammar[i].lhs - 'A'], 
                              follow_size[grammar[i].lhs - 'A'], 
                              follow[idx], 
                              &follow_size[idx]);
                }
            }
        }
    }
}

