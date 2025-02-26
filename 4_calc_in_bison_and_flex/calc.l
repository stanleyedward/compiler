%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include "calc.tab.h"  /* Bison header file for token definitions */
%}

%%
[ \t]+                  ;                           /* Ignore whitespace */
[0-9]+\.[0-9]+          { yylval.fval = atof(yytext); return T_FLOAT; }
[0-9]+                  { yylval.fval = atof(yytext); return T_FLOAT; }
"\n"                   { return T_NEWLINE; }
"+"                    { return T_PLUS; }
"-"                    { return T_MINUS; }
"("                    { return T_LEFT; }
")"                    { return T_RIGHT; }
"exit"                 { return T_QUIT; }
"quit"                 { return T_QUIT; }
"SINH"                 { return T_SINH; }
"COSH"                 { return T_COSH; }
"ASIN"                 { return T_ASIN; }
"ACOS"                 { return T_ACOS; }
.                      { printf("Unknown character: %s\n", yytext); }
%%

