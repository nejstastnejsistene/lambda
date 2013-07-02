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

// Create a deep copy of a lamVal.
lamVal *copyLamVal(lamVal *x) {
    lamVal *ret, *p;

    // Allocate the inital lamVal.
    ret = p = malloc(sizeof(lamVal));

    int done = 0;
    while (!done) {

        p->type = x->type;
        switch (p->type) {

            case VAR:
                // Base case.
                p->varName = copyString(x->varName);
                done = 1;
                break;

            case ABS:
                p->absVar = copyString(x->absVar);
                // Allocate body and recur.
                p->absBody = malloc(sizeof(lamVal));
                p = p->absBody;
                x = x->absBody;
                break;

            case APP:
                // Unfortunately not a tail call.
                p->appFunc = copyLamVal(x->appFunc);
                // Allocate arg and recur.
                p->appArg = malloc(sizeof(lamVal));
                p = p->appArg;
                x = x->appArg;
                break;
        }
    }

    return ret;
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

// Substitute all instances of k with v in x, until k is rebound.
lamVal *substitute(char *k, lamVal *v, lamVal *x) {
    char *tmp0;
    lamVal *tmp1, *tmp2;

    switch (x->type) {

        case VAR:
            return copyLamVal(strcmp(x->varName, k) == 0 ? v : x);

        case ABS:
            // If k has been rebound by x, return x unmodified.
            if (strcmp(x->varName, k) == 0) {
                return copyLamVal(x);    
            // Otherwise continue substituting.
            } else {
                tmp0 = copyString(x->absVar);
                tmp2 = substitute(k, v, x->absBody);
                return newAbs(tmp0, tmp2);
            }

        case APP:
            // Recur on copies of both branches.
            tmp1 = substitute(k, v, x->appFunc);
            tmp2 = substitute(k, v, x->appArg);
            return apply(tmp1, tmp2);

    };
    return NULL;
}

// Create a nice string representation of a lamVal object.
// Var x => x
// Abs x y => (λx.y)
// App x y => (x y)
char *showLamVal(lamVal *x) {
    int length;
    char *buf, *tmp;

    switch (x->type) {

        case VAR:
            // A variable is its own representation.
            buf = copyString(x->varName);
            break;

        case ABS:
            tmp = showLamVal(x->absBody);

            // Allocate buffer and copy into it.
            length = strlen(x->absVar) + strlen(tmp) + 5;
            buf = malloc(length + 1);
            sprintf(buf, "(\xce\xbb%s.%s)", x->absVar, tmp);

            free(tmp);
            break;

        case APP:
            buf = showApp(x, 1);
            break;
    }

    return buf;
}

// In order to nicely display repeated left-associative applications, the
// parens flag controls whether or not there should be parentheses around
// the application. This way, something like ((((a b) c) d) e) will be
// displayed as (a b c d e).
char *showApp(lamVal *x, int parens) {
    int length;
    char *buf, *func, *arg;

    // Show an application without parentheses.
    if (x->appFunc->type == APP) {
        func = showApp(x->appFunc, 0);
    // Show the normal way.
    } else {
        func = showLamVal(x->appFunc);
    }
    arg = showLamVal(x->appArg);

    // Allocate an appropriately sized buffer, and copy into it.
    length = strlen(func) + strlen(arg) + (parens ? 3 : 1);
    buf = malloc(length + 1);
    sprintf(buf, parens ? "(%s %s)" : "%s %s", func, arg);

    // Free the component strings now that we're done with them.
    free(func);
    free(arg);

    return buf;
}

int main() {
    lamVal *two, *four, *sixteen, *foo;

    two = toChurch(2);
    four = toChurch(4);
    sixteen = apply(two, four);
    
    foo = newApp(newVar("a"), newVar("b"));
    foo = newApp(foo, newVar("c"));
    foo = newApp(foo, newVar("d"));
    foo = newApp(foo, newVar("e"));

    printf("2 => %s\n", showLamVal(two));
    printf("4 => %s\n", showLamVal(four));
    printf("(2 4) => %s\n", showLamVal(newApp(two, four)));
    printf("(2 4) => %s\n", showLamVal(sixteen));
    printf("Left associatives applications:\n");
    printf("(a b c d e) => %s\n", showLamVal(foo));

    freeLamVal(two);
    freeLamVal(four);
    freeLamVal(sixteen);
    freeLamVal(foo);

    return 0;
}
