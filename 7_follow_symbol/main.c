#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROD 10
#define MAX_SYMBOLS 10

struct Production {
    char lhs;
    char rhs[MAX_SYMBOLS];
};

int num_productions = 0;
struct Production grammar[MAX_PROD];

char first[256][MAX_SYMBOLS] = {0};
char follow[256][MAX_SYMBOLS] = {0};

int is_terminal(char c) {
    return !(isupper(c) || c == 'e' || c == 't');
}

void add_char(char *set, char c) {
    if (c == '\0') return;
    if (!strchr(set, c)) {
        int len = strlen(set);
        set[len] = c;
        set[len + 1] = '\0';
    }
}

void compute_string_first(char *beta, char *result) {
    result[0] = '\0';
    int can_derive_epsilon = 1;

    for (int i = 0; beta[i] != '\0' && can_derive_epsilon; i++) {
        char symbol = beta[i];
        if (is_terminal(symbol)) {
            add_char(result, symbol);
            can_derive_epsilon = 0;
            break;
        } else {
            char *fs = first[symbol];
            for (int j = 0; fs[j] != '\0'; j++) {
                if (fs[j] != 'ε') {
                    add_char(result, fs[j]);
                }
            }
            if (!strchr(fs, 'ε')) {
                can_derive_epsilon = 0;
            }
        }
    }

    if (can_derive_epsilon) {
        add_char(result, 'ε');
    }
}

void compute_first() {
    int changes;
    do {
        changes = 0;
        for (int i = 0; i < num_productions; i++) {
            char lhs = grammar[i].lhs;
            char *rhs = grammar[i].rhs;

            char current_first[MAX_SYMBOLS];
            strcpy(current_first, first[lhs]);

            int can_derive_epsilon = 1;
            for (int j = 0; rhs[j] != '\0' && can_derive_epsilon; j++) {
                char symbol = rhs[j];
                if (is_terminal(symbol)) {
                    add_char(first[lhs], symbol);
                    can_derive_epsilon = 0;
                    break;
                } else {
                    char *fs = first[symbol];
                    for (int k = 0; fs[k] != '\0'; k++) {
                        if (fs[k] != 'ε') {
                            add_char(first[lhs], fs[k]);
                        }
                    }
                    if (!strchr(fs, 'ε')) {
                        can_derive_epsilon = 0;
                    }
                }
            }
            if (can_derive_epsilon) {
                add_char(first[lhs], 'ε');
            }

            if (strcmp(current_first, first[lhs]) != 0) {
                changes = 1;
            }
        }
    } while (changes);
}

void compute_follow() {
    add_char(follow['E'], '$');

    int changes;
    do {
        changes = 0;
        for (int i = 0; i < num_productions; i++) {
            char A = grammar[i].lhs;
            char *alpha = grammar[i].rhs;

            for (int j = 0; alpha[j] != '\0'; j++) {
                char B = alpha[j];
                if (is_terminal(B)) continue;

                char beta[MAX_SYMBOLS];
                strcpy(beta, alpha + j + 1);

                char first_beta[MAX_SYMBOLS] = {0};
                compute_string_first(beta, first_beta);

                for (int k = 0; first_beta[k] != '\0'; k++) {
                    if (first_beta[k] != 'ε') {
                        if (!strchr(follow[B], first_beta[k])) {
                            add_char(follow[B], first_beta[k]);
                            changes = 1;
                        }
                    }
                }

                if (strchr(first_beta, 'ε')) {
                    char *follow_A = follow[A];
                    for (int k = 0; follow_A[k] != '\0'; k++) {
                        if (!strchr(follow[B], follow_A[k])) {
                            add_char(follow[B], follow_A[k]);
                            changes = 1;
                        }
                    }
                }
            }
        }
    } while (changes);
}

int main() {
    struct Production prods[] = {
        {'E', "Te"}, {'e', "+Te"}, {'e', ""}, {'T', "Ft"},
        {'t', "*Ft"}, {'t', ""}, {'F', "(E)"}, {'F', "i"}
    };
    num_productions = sizeof(prods) / sizeof(prods[0]);
    memcpy(grammar, prods, sizeof(prods));

    for (int c = 0; c < 256; c++) {
        if (is_terminal(c) && c != 'ε') {
            first[c][0] = c;
            first[c][1] = '\0';
        }
    }

    compute_first();
    compute_follow();

    char *non_terminals = "EeTtF";
    for (int i = 0; non_terminals[i]; i++) {
        char nt = non_terminals[i];
        printf("FOLLOW(%c) = { ", nt);
        for (int j = 0; follow[nt][j]; j++) {
            printf("%c ", follow[nt][j]);
        }
        printf("}\n");
    }

    return 0;
}