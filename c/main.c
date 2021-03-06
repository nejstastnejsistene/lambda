#include<stdio.h>

#include "church.h"
#include "lambda.h"

#include "parser.h"
#include "lexer.h"

int yyparse(lamVal **expression, yyscan_t scanner);

lamVal *parse(const char *input) {
    lamVal *expr;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        return NULL;
    }

    state = yy_scan_string(input, scanner);

    if (yyparse(&expr, scanner)) {
        return NULL;
    }

    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);

    return expr;
}

int main() {

    lamVal *sixteen, *twoFour, *fourSquared;

    sixteen = toChurch(16);
    twoFour = parse("(2 4)");
    fourSquared = eval(twoFour);
    printf("16    => %s\n", showLamVal(sixteen));
    printf("(2 4) => %s\n", showLamVal(fourSquared));
    printf("16 == (2 4) => %s\n", eq(sixteen, fourSquared) ? "True" : "False");

    freeLamVal(sixteen);
    freeLamVal(twoFour);
    freeLamVal(fourSquared);

    lamVal *tmp, *result;
    char buffer[1024];

    while (1) {
        printf("λ>");
        fgets(buffer, 1024, stdin); 
        if (strlen(buffer) > 1 && (tmp = parse(buffer))) {
            result = eval(tmp);
            printf("%s\n", showLamVal(result));
            freeLamVal(tmp);
            freeLamVal(result);
        }
    }

    return 0;
}
