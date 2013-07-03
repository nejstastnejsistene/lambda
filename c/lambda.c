#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "alpha.h"
#include "lambda.h"
#include "llist.h"


lamVal *newVar(char *name) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = VAR;
    x->varName = strdup(name);
    return x;
}

lamVal *newAbs(char *varName, lamVal *body) {
    lamVal *x = malloc(sizeof(lamVal));
    x->type = ABS;
    x->absVar = strdup(varName);
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
                p->varName = strdup(x->varName);
                done = 1;
                break;

            case ABS:
                p->absVar = strdup(x->absVar);
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

// Free a lamVal completely and entirely. To avoid crashing the runtime, all
// functions should treat lamVals as immutable and return completely fresh
// copies of whatever they are modified.
void freeLamVal(lamVal *x) {
    lamVal *next;

    int done = 0;
    while (!done) {

        // Free the contents, and set the next lamVal to recur on.
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

        // Free the current lamVal and recur.
        free(x);
        x = next;
    }
}

// Compare two lamVals for alpha-equivalence.
int eq(lamVal *x, lamVal *y) {
    return eq_(x, y, 0, NULL, NULL);
}

int eq_(lamVal *x, lamVal *y, int n, node *xEnv, node *yEnv) {
    int xVal, yVal;

    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {

        case VAR:
            xVal = lookup(x->varName, xEnv);
            yVal = lookup(y->varName, yEnv);

            // Both variables must be either bound or not bound,
            // not one of each.
            if ((xVal < 0) != (yVal < 0)) {
                return 0;
            }

            // Compare free variables by value.
            if (xVal < 0) {
                return strcmp(x->varName, y->varName) == 0;

            // Compare bound variables by their position.
            } else {
                return xVal == yVal;
            }

        case ABS:
            // Record the positions of the bound variables.
            xEnv = put(x->absVar, n, xEnv);
            yEnv = put(y->absVar, n, yEnv);
            return eq_(x->absBody, y->absBody, n+1, xEnv, yEnv);

        case APP:
            // Simply recur on both sides of the application.
            return eq_(x->appFunc, y->appFunc, n, xEnv, yEnv)
                && eq_(x->appArg,  y->appArg,  n, xEnv, yEnv);
    }

    return 0;
}

lamVal *eval(lamVal *x) {
    lamVal *ret;
    switch (x->type) {
        case VAR:
            ret = copyLamVal(x);
            break;
        case ABS:
            ret = newAbs(strdup(x->absVar), eval(x->absBody));
            break;
        case APP:
            ret = apply(eval(x->appFunc), eval(x->appArg));
            break;
    }
    return ret;
}

// Apply two lamVals to each other. If the first argument is an abstraction,
// it will perform the appropriate substitution. Otherwise it will return a
// simple App.
lamVal *apply(lamVal *x, lamVal *y) {
    lamVal *tmp, *ret;

    if (x->type == ABS) {
        // Create a temporary copy with no namespace collisions.
        tmp = alpha(x->absBody, y);
        // Perform the substitution.
        ret = substitute(x->absVar, y, tmp);
        // Free the temporarty copy.
        freeLamVal(tmp);
    } else {
        ret = newApp(copyLamVal(x), copyLamVal(y));
    }

    return ret;
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
                tmp0 = strdup(x->absVar);
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
            buf = strdup(x->varName);
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
