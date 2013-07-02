#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "lambda.h"
#include "alpha.h"
#include "llist.h"

// Rename x with respect to y. Returns an alpha-equivalent copy of x whose
// set of variable names does not intersect with that of y.
lamVal *alpha(lamVal *x, lamVal *y) {
    int len;
    char *k, *v;
    node *xVars, *yVars, *ks, *vs, *namespace;

    // Find vars bound by both x and y.
    xVars = nub(boundVars(x));
    yVars = nub(boundVars(y));
    ks = intersection(xVars, yVars);

    // Return early if there are no collisions.
    len = length(ks);
    if (!len) {
        return x;
    }

    // Get new variables to replace the collisions.
    namespace = nub(allVars(x));
    append(namespace, nub(allVars(y)));
    vs = newVars(len, namespace);

    k = ks->head;
    v = vs->head;

    // Rename each of the variables.
    lamVal *ret = copyLamVal(x);
    for (; ks != NULL; ks = ks->tail, vs = vs->tail) {
        ret = rename_(k, v, ret);
    }

    return ret;
}

// Rename k to v in x, performing the modification in place.
lamVal *rename_(char *k, char *v, lamVal *x) {
    lamVal *ret = x;

    int done = 0;
    while (!done) {
        switch (x->type) {

            case VAR:
                if (strcmp(k, x->varName) == 0) {
                    x->varName = copyString(v);
                }
                // Exit the loop.
                done = 1;
                break;

            case ABS:
                if (strcmp(k, x->absVar) == 0) {
                    x->absVar = copyString(v);
                }
                // Recur on the body.
                x = x->absBody;
                break;

            case APP:
                // Rename the func using conventional recursion.
                x->appFunc = rename_(k, v, x->appFunc);

                // Recur on the arg.
                x = x->appArg;
                break;
        }
    }

    return ret;
}

// Return all variables bound in x.
node *boundVars(lamVal *x) {
    return getVars(x, 0, NULL);
}

// Return all variables, free and bound, in x.
node *allVars(lamVal *x) {
    return getVars(x, 1, NULL);
}

// Return variables in x.
// Args:
//  freevars    Whether to include free variables.
//  vars        The llist node to store the results in.
node *getVars(lamVal *x, int freeVars, node *vars) {
    int searching = 1;
    while (searching) {
        switch (x->type) {
            case VAR:
                if (freeVars) {
                    vars = cons(x->varName, vars);
                }
                searching = 0;
                break;
            case ABS:
                vars = cons(x->absVar, vars);
                x = x->absBody;
                break;
            case APP:
                vars = getVars(x->appFunc, freeVars, vars);
                x = x->appArg;
                break;
        }
    }
    return vars;
}

// Returns n variables that do not collide with excluding.
node *newVars(int n, node *excluding) {
    node *ret = NULL;
    size_t bufSize = 1024;
    char buf[bufSize];
    int numChars = 0, i;
    
    // Clear the buffer and so the next string generated will be "a".
    memset(&buf, 0, bufSize);
    buf[0] = 'a' - 1;

    // Generate all permutations of lowercase ascii characters.
    while (n) {

        // Overflow when the current char reaches 'z'.
        for (i = numChars; i > 0 && buf[i] == 'z'; i--) {
            buf[i] = 'a';
        }

        // Increment the current char if it hasn't reached 'z'.
        if (buf[i] < 'z') {
            buf[i]++;

        // When all chars have overflowed, add a new char.
        } else {
            buf[i] = 'a';
            buf[++numChars] = 'a';
        }

        // Check if the string should be excluded.
        if (!contains(buf, excluding)) {

            // Append a copy of the string to the list.
            ret = cons(copyString(buf), ret);

            // Break if enough strings have been generated.
            if (!--n) {
                break;
            }
        }
    }
    return ret;
}
