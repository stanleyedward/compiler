%{
#include <stdio.h>
#include <stdlib.h>

int count = 0;       

extern int yylex(void);
void yyerror(const char *s);
%}

%token CHARACTER

%%

input:
      /* empty */
    | input CHARACTER { count++; }
    ;

%%

int main(void) {
    yyparse();
    printf("Total characters: %d\n", count);
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(1);
}
