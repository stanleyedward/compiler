#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 100
#define MAX_RULES 100
#define MAX_RHS 20

// Symbol types
#define NON_TERMINAL 0
#define TERMINAL 1
#define EPSILON 2
#define END_MARKER 3

// Structure for symbols
typedef struct {
    char name[10];
    int type;
} Symbol;

// Structure for grammar rules (productions)
typedef struct {
    int lhs;                // Left hand side non-terminal index
    int rhs[MAX_RHS];       // Right hand side symbols indices
    int rhs_length;
} Rule;

// Grammar representation
Symbol symbols[MAX_SYMBOLS];
Rule rules[MAX_RULES];
int num_symbols = 0;
int num_rules = 0;
int start_symbol = 0;

// Sets for FIRST and FOLLOW
bool first[MAX_SYMBOLS][MAX_SYMBOLS];
bool follow[MAX_SYMBOLS][MAX_SYMBOLS];
bool first_calculated[MAX_SYMBOLS] = {false};
bool follow_calculated[MAX_SYMBOLS] = {false};

// Function to add a symbol
int add_symbol(const char* name, int type) {
    for (int i = 0; i < num_symbols; i++) {
        if (strcmp(symbols[i].name, name) == 0) {
            return i;
        }
    }
    
    strcpy(symbols[num_symbols].name, name);
    symbols[num_symbols].type = type;
    return num_symbols++;
}

// Function to add a rule
void add_rule(const char* lhs, const char** rhs, int rhs_length) {
    int lhs_idx = add_symbol(lhs, NON_TERMINAL);
    
    rules[num_rules].lhs = lhs_idx;
    rules[num_rules].rhs_length = rhs_length;
    
    for (int i = 0; i < rhs_length; i++) {
        // Automatically determine the type: uppercase letters are non-terminals
        int type = (rhs[i][0] >= 'A' && rhs[i][0] <= 'Z') ? NON_TERMINAL : TERMINAL;
        if (strcmp(rhs[i], "ε") == 0) type = EPSILON;
        
        rules[num_rules].rhs[i] = add_symbol(rhs[i], type);
    }
    
    num_rules++;
}

// Calculate FIRST set for a symbol
void calculate_first(int symbol) {
    // If already calculated
    if (first_calculated[symbol]) return;
    
    first_calculated[symbol] = true;
    
    // If terminal, FIRST contains only itself
    if (symbols[symbol].type == TERMINAL || symbols[symbol].type == EPSILON || symbols[symbol].type == END_MARKER) {
        first[symbol][symbol] = true;
        return;
    }
    
    // For non-terminals, check all rules
    for (int i = 0; i < num_rules; i++) {
        if (rules[i].lhs == symbol) {
            // If ε is in the right hand side
            if (rules[i].rhs_length == 1 && symbols[rules[i].rhs[0]].type == EPSILON) {
                int epsilon_idx = rules[i].rhs[0];
                first[symbol][epsilon_idx] = true;
                continue;
            }
            
            // Process each symbol in the right hand side
            for (int j = 0; j < rules[i].rhs_length; j++) {
                int current = rules[i].rhs[j];
                
                // Calculate FIRST for this symbol if not done yet
                if (!first_calculated[current]) {
                    calculate_first(current);
                }
                
                // Add all symbols from FIRST(current) to FIRST(symbol) except ε
                for (int k = 0; k < num_symbols; k++) {
                    if (first[current][k] && symbols[k].type != EPSILON) {
                        first[symbol][k] = true;
                    }
                }
                
                // If ε is not in FIRST(current), break
                bool has_epsilon = false;
                for (int k = 0; k < num_symbols; k++) {
                    if (first[current][k] && symbols[k].type == EPSILON) {
                        has_epsilon = true;
                        break;
                    }
                }
                
                if (!has_epsilon) break;
                
                // If we reached the last symbol and all can derive ε, add ε to FIRST(symbol)
                if (j == rules[i].rhs_length - 1) {
                    for (int k = 0; k < num_symbols; k++) {
                        if (symbols[k].type == EPSILON) {
                            first[symbol][k] = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}

// Calculate FOLLOW set for a symbol
void calculate_follow(int symbol) {
    // If already calculated
    if (follow_calculated[symbol]) return;
    
    follow_calculated[symbol] = true;
    
    // Add $ to FOLLOW of start symbol
    if (symbol == start_symbol) {
        for (int i = 0; i < num_symbols; i++) {
            if (symbols[i].type == END_MARKER) {
                follow[symbol][i] = true;
                break;
            }
        }
    }
    
    // Find all occurrences of this symbol in the right hand sides
    for (int i = 0; i < num_rules; i++) {
        for (int j = 0; j < rules[i].rhs_length; j++) {
            if (rules[i].rhs[j] == symbol) {
                // Case 1: A → αBβ - add FIRST(β) to FOLLOW(B)
                if (j < rules[i].rhs_length - 1) {
                    int next = rules[i].rhs[j + 1];
                    
                    // Make sure FIRST is calculated
                    if (!first_calculated[next]) {
                        calculate_first(next);
                    }
                    
                    // Add all from FIRST(next) to FOLLOW(symbol) except ε
                    bool has_epsilon = false;
                    for (int k = 0; k < num_symbols; k++) {
                        if (first[next][k]) {
                            if (symbols[k].type == EPSILON) {
                                has_epsilon = true;
                            } else {
                                follow[symbol][k] = true;
                            }
                        }
                    }
                    
                    // If ε is in FIRST(next), proceed to next symbol or apply Case 2
                    if (has_epsilon) {
                        // If this is the last symbol in the rule, apply Case 2
                        if (j + 1 == rules[i].rhs_length - 1) {
                            // Case 2: A → αB or A → αBβ where ε is in FIRST(β)
                            // Add FOLLOW(A) to FOLLOW(B)
                            int lhs = rules[i].lhs;
                            
                            // Avoid infinite recursion
                            if (lhs != symbol) {
                                if (!follow_calculated[lhs]) {
                                    calculate_follow(lhs);
                                }
                                
                                // Copy all from FOLLOW(lhs) to FOLLOW(symbol)
                                for (int k = 0; k < num_symbols; k++) {
                                    if (follow[lhs][k]) {
                                        follow[symbol][k] = true;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    // Case 2: A → αB
                    // Add FOLLOW(A) to FOLLOW(B)
                    int lhs = rules[i].lhs;
                    
                    // Avoid infinite recursion
                    if (lhs != symbol) {
                        if (!follow_calculated[lhs]) {
                            calculate_follow(lhs);
                        }
                        
                        // Copy all from FOLLOW(lhs) to FOLLOW(symbol)
                        for (int k = 0; k < num_symbols; k++) {
                            if (follow[lhs][k]) {
                                follow[symbol][k] = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Print FIRST and FOLLOW sets
void print_sets() {
    printf("FIRST sets:\n");
    for (int i = 0; i < num_symbols; i++) {
        if (symbols[i].type == NON_TERMINAL) {
            printf("FIRST(%s) = { ", symbols[i].name);
            bool first_printed = false;
            for (int j = 0; j < num_symbols; j++) {
                if (first[i][j]) {
                    if (first_printed) printf(", ");
                    printf("%s", symbols[j].name);
                    first_printed = true;
                }
            }
            printf(" }\n");
        }
    }
    
    printf("\nFOLLOW sets:\n");
    for (int i = 0; i < num_symbols; i++) {
        if (symbols[i].type == NON_TERMINAL) {
            printf("FOLLOW(%s) = { ", symbols[i].name);
            bool first_printed = false;
            for (int j = 0; j < num_symbols; j++) {
                if (follow[i][j]) {
                    if (first_printed) printf(", ");
                    printf("%s", symbols[j].name);
                    first_printed = true;
                }
            }
            printf(" }\n");
        }
    }
}

int main() {
    // Initialize example grammar
    add_symbol("$", END_MARKER);  // End marker
    
    // // Example grammar:
    // // E → T E'
    // // E' → + T E' | ε
    // // T → F T'
    // // T' → * F T' | ε
    // // F → ( E ) | id
    
    // const char* rhs1[] = {"T", "E'"};
    // add_rule("E", rhs1, 2);
    
    // const char* rhs2[] = {"+", "T", "E'"};
    // add_rule("E'", rhs2, 3);
    
    // const char* rhs3[] = {"ε"};
    // add_rule("E'", rhs3, 1);
    
    // const char* rhs4[] = {"F", "T'"};
    // add_rule("T", rhs4, 2);
    
    // const char* rhs5[] = {"*", "F", "T'"};
    // add_rule("T'", rhs5, 3);
    
    // const char* rhs6[] = {"ε"};
    // add_rule("T'", rhs6, 1);
    
    // const char* rhs7[] = {"(", "E", ")"};
    // add_rule("F", rhs7, 3);
    
    // const char* rhs8[] = {"id"};
    // add_rule("F", rhs8, 1);

    // Simple grammar:
    // S → A B
    // A → a | ε
    // B → b
    
    const char* rhs1[] = {"A", "B"};
    add_rule("S", rhs1, 2);
    
    const char* rhs2[] = {"a"};
    add_rule("A", rhs2, 1);
    
    const char* rhs3[] = {"ε"};
    add_rule("A", rhs3, 1);
    
    const char* rhs4[] = {"b"};
    add_rule("B", rhs4, 1);
    
    // Find the index of the start symbol 'E'
    for (int i = 0; i < num_symbols; i++) {
        if (strcmp(symbols[i].name, "S") == 0) {
            start_symbol = i;
            break;
        }
    }
    
    // Calculate FIRST sets for all non-terminals
    for (int i = 0; i < num_symbols; i++) {
        if (symbols[i].type == NON_TERMINAL) {
            calculate_first(i);
        }
    }
    
    // Calculate FOLLOW sets for all non-terminals
    for (int i = 0; i < num_symbols; i++) {
        if (symbols[i].type == NON_TERMINAL) {
            calculate_follow(i);
        }
    }
    
    // Print results
    print_sets();
    
    return 0;
}