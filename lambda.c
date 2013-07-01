#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "lambda.h"
#include "alpha.h"

lamVal *newVar(char *name) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = VAR;
    x->varName = name;
    return x;
}

lamVal *newAbs(char *varName, lamVal *body) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = ABS;
    x->absVar = varName;
    x->absBody = body;
    return x;
}

lamVal *newApp(lamVal *func, lamVal *arg) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = APP;
    x->appFunc = func;
    x->appArg = arg;
    return x;
}

lamVal *copyLamVal(lamVal *x) {
    lamVal *y = malloc(sizeof(lamVal));
    y->type = x->type;
    switch (x->type) {
        case VAR:
            y->varName = malloc(strlen(x->varName) + 1);
            strcpy(y->varName, x->varName);
            break;
        case ABS:
            y->absVar = malloc(strlen(x->absVar) + 1);
            strcpy(y->absVar, x->absVar);
            y->absBody = copyLamVal(x->absBody);
            break;
        case APP:
            y->appFunc = copyLamVal(x->appFunc);
            y->appArg = copyLamVal(x->appArg);
            break;
    };
    return y;
}

void freeLamVal(lamVal *x) {
    switch (x->type) {
        case VAR:
            free(x->varName);
            break;
        case ABS:
            free(x->absVar);
            freeLamVal(x->absBody);
            break;
        case APP:
            freeLamVal(x->appFunc);
            freeLamVal(x->appArg);
            break;
    };
    free(x);
}

lamVal *apply(lamVal *x, lamVal *y) {
    if (x->type == ABS) {
        return substitute(x->absVar, y, alpha(y, x->absBody));
    } else {
        return newApp(x, y);
    }
}

lamVal *substitute(char *k, lamVal *v, lamVal *x) {
    lamVal *tmp1, *tmp2;
    switch (x->type) {
        case VAR:
            if (strcmp(x->varName, k) == 0) {
                return v;
            } else {
                return x;
            }
        case ABS:
            if (strcmp(x->varName, k) == 0) {
                return x;    
            } else {
                tmp1 = copyLamVal(x);
                tmp1->absBody = substitute(k, v, x->absBody);
                return tmp1;
            }
        case APP:
            tmp1 = substitute(k, v, x->appFunc);
            tmp2 = substitute(k, v, x->appArg);
            return newApp(tmp1, tmp2);

    };
    return NULL;
}

char *showLamVal(lamVal *x) {
    int length;
    char *buf, *tmp1, *tmp2;
    switch (x->type) {
        case VAR:
            length = strlen(x->varName) + 1;
            buf = malloc(length + 1);
            strcpy(buf, x->varName);
            break;
        case ABS:
            tmp1 = showLamVal(x->absBody);
            length = strlen(x->absVar) + strlen(tmp1) + 5;
            buf = malloc(length + 1);
            sprintf(buf, "(\xce\xbb%s.%s)", x->absVar, tmp1);
            free(tmp1);
            break;
        case APP:
            tmp1 = showLamVal(x->appFunc);
            tmp2 = showLamVal(x->appArg);
            length = strlen(tmp1) + strlen(tmp2) + 3;
            buf = malloc(length + 1);
            sprintf(buf, "(%s %s)", tmp1, tmp2);
            free(tmp1);
            free(tmp2);
            break;
    }
    return buf;
}

int main() {
    lamVal *a, *b, *c, *id, *idc;

    a = newVar("a");
    b = newVar("b");
    c = newVar("c");

    id = newAbs("a", a);
    idc = apply(id, c);

    printf("%s\n", showLamVal(a));
    printf("%s\n", showLamVal(b));
    printf("%s\n", showLamVal(c));
    printf("%s\n", showLamVal(id));
    printf("%s\n", showLamVal(idc));

    return 0;
}
