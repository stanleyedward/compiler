#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 100

int commonPrefixLength(const char *a, const char *b) {
    int i = 0;
    while(a[i] && b[i] && a[i] == b[i])
        i++;
    return i;
}

int main() {
    int n;
    printf("Enter number of alternatives: ");
    scanf("%d", &n);

    // Allocate memory for the alternatives
    char **prods = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        prods[i] = malloc(MAX_LEN);
        printf("Enter alternative %d: ", i + 1);
        scanf("%s", prods[i]);
    }

    int common = strlen(prods[0]);
    for (int i = 1; i < n; i++) {
        int cp = commonPrefixLength(prods[0], prods[i]);
        if (cp < common)
            common = cp;
    }
    if (common == 0) {
        printf("No common prefix found.\n");
        for (int i = 0; i < n; i++)
            free(prods[i]);
        free(prods);
        return 0;
    }
    
    char prefix[MAX_LEN];
    strncpy(prefix, prods[0], common);
    prefix[common] = '\0';
    
    // Output the factored productions
    printf("A -> %sX\n", prefix);
    printf("X -> ");
    for (int i = 0; i < n; i++) {
        if (strlen(prods[i]) == common)
            printf("\u03b5");
        else
            printf("%s", prods[i] + common);
        if (i < n - 1)
            printf(" | ");
    }
    printf("\n");

    // Free allocated memory
    for (int i = 0; i < n; i++)
        free(prods[i]);
    free(prods);
    return 0;
}
