%{

#include <stdio.h>

#include "lambda.h"
#include "parser.h"

%}

%option outfile="lexer.c" header-file="lexer.h"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

digit       [0-9]
letter      [a-zA-Z_]

%%

[ \r\n\t]*  /* skip whitespace */

"λ"         { return TOKEN_LAMBDA;  }
"\\"        { return TOKEN_LAMBDA;  }
"."         { return TOKEN_DOT;     } 
"("         { return TOKEN_LPAREN;  }
")"         { return TOKEN_RPAREN;  }

{digit}+    { yylval->numVal = atoi(yytext);
              return TOKEN_NUM;
            }

.           { yylval->idVal = strdup(yytext);
              return TOKEN_VAR;
            }

%%

int yyerror(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}
