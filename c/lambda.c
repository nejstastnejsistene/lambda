#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "lambda.h"
#include "alpha.h"
#include "church.h"

// helper method
char *copyString(char *x) {
    char *ret = malloc(strlen(x) + 1);
    strcpy(ret, x);
    return ret;
}

lamVal *newVar(char *name) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = VAR;
    x->varName = copyString(name);
    return x;
}

lamVal *newAbs(char *varName, lamVal *body) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = ABS;
    x->absVar = copyString(varName);
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
            y->varName = copyString(x->varName);
            break;
        case ABS:
            y->absVar = copyString(x->absVar);
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
    lamVal *next;
    int done = 0;
    while (!done) {
        switch (x->type) {
            case VAR:
                free(x->varName);
                done = 1;
                break;
            case ABS:
                free(x->absVar);
                next = x->absBody;
                break;
            case APP:
                freeLamVal(x->appFunc);
                next = x->appArg;
                break;
        };
        free(x);
        x = next;
    }
}

lamVal *apply(lamVal *x, lamVal *y) {
    lamVal *tmp, *ret;
    if (x->type == ABS) {
        tmp = alpha(x->absBody, y);
        ret = substitute(x->absVar, y, tmp);
        freeLamVal(tmp);
        return ret;
    } else {
        return newApp(copyLamVal(x), copyLamVal(y));
    }
}

lamVal *substitute(char *k, lamVal *v, lamVal *x) {
    lamVal *tmp1, *tmp2;
    switch (x->type) {
        case VAR:
            if (strcmp(x->varName, k) == 0) {
                return copyLamVal(v);
            } else {
                return copyLamVal(x);
            }
        case ABS:
            if (strcmp(x->varName, k) == 0) {
                return copyLamVal(x);    
            } else {
                tmp1 = copyLamVal(x);
                tmp1->absBody = substitute(k, v, tmp1->absBody);
                return tmp1;
            }
        case APP:
            tmp1 = substitute(k, v, copyLamVal(x->appFunc));
            tmp2 = substitute(k, v, copyLamVal(x->appArg));
            return apply(tmp1, tmp2);

    };
    return NULL;
}

char *showLamVal(lamVal *x) {
    int length;
    char *buf, *tmp1, *tmp2;
    switch (x->type) {
        case VAR:
            buf = copyString(x->varName);
            x = NULL;
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
    lamVal *two, *four, *sixteen;

    two = toChurch(2);
    four = toChurch(4);
    sixteen = apply(two, four);

    printf("%s\n", showLamVal(two));
    printf("%s\n", showLamVal(four));
    printf("%s\n", showLamVal(newApp(two, four)));
    printf("%s\n", showLamVal(sixteen));

    freeLamVal(two);
    freeLamVal(four);
    freeLamVal(sixteen);

    return 0;
}
