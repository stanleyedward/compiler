%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>   /* For math functions: sinh, cosh, asin, acos */
void yyerror(const char *s);
int yylex();
%}

%union {
    float fval;
}

%token <fval> T_INT    /* Not used in this version; numbers come in as T_FLOAT */
%token <fval> T_FLOAT
%token T_PLUS T_MINUS T_LEFT T_RIGHT T_NEWLINE T_QUIT
%token T_SINH T_COSH T_ASIN T_ACOS

%left T_PLUS T_MINUS

%type <fval> expression

%start calculation

%%

calculation:
      /* empty */
    | calculation line
    ;

line:
      T_NEWLINE
    | expression T_NEWLINE { printf("\tResult: %f\n", $1); }
    | T_QUIT T_NEWLINE     { printf("bye!\n"); exit(0); }
    ;

expression:
      T_FLOAT                         { $$ = $1; }
    | T_LEFT expression T_RIGHT         { $$ = $2; }
    | expression T_PLUS expression      { $$ = $1 + $3; }
    | expression T_MINUS expression     { $$ = $1 - $3; }
    /* Function calls – note that the argument is parsed as an expression */
    | T_SINH T_LEFT expression T_RIGHT    { $$ = sinh($3); }
    | T_COSH T_LEFT expression T_RIGHT    { $$ = cosh($3); }
    | T_ASIN T_LEFT expression T_RIGHT    { $$ = asin($3); }
    | T_ACOS T_LEFT expression T_RIGHT    { $$ = acos($3); }
    ;

%%

int main(void) {
    printf("Enter expression:\n");
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
