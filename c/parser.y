%{

#include "lambda.h"
#include "church.h"
#include "parser.h"
#include "lexer.h"

int yyerror(yyscan_t scanner, lamVal **expression, const char* msg);

%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

}

%output  "parser.c"
%defines "parser.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { lamVal **expression}
%parse-param { yyscan_t scanner }

%union {
    char *idVal;
    int numVal;
    lamVal *expression;
}

%token TOKEN_LAMBDA
%token TOKEN_DOT
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token <idVal> TOKEN_VAR
%token <numVal> TOKEN_NUM
%token UNKNOWN

%type <expression> expr

%%

program
    : expr { *expression = $1; }
    ;

expr
    : TOKEN_VAR                                 { $$ = newVar($1);     }
    | TOKEN_LAMBDA TOKEN_VAR TOKEN_DOT expr     { $$ = newAbs($2, $4); }
    | TOKEN_LPAREN expr expr TOKEN_RPAREN       { $$ = newApp($2, $3); }
    | TOKEN_LPAREN expr TOKEN_RPAREN            { $$ = $2;             }
    | TOKEN_NUM                                 { $$ = toChurch($1);   }
    ;

%%
