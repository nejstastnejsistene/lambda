#include<stdlib.h>
#include<string.h>

#include "lambda.h"
#include "alpha.h"
#include "llist.h"

cmp_t *_strcmp = (cmp_t*)&strcmp;

lamVal *alpha(lamVal *x, lamVal *y) {
    int len;
    char *k, *v;
    node *xVars, *yVars, *ks, *vs, *namespace;

    xVars = nubBy(_strcmp, boundVars(x));
    yVars = nubBy(_strcmp, boundVars(y));
    ks = intersectBy(_strcmp, xVars, yVars);

    len = length(ks);
    if (!len) {
        return x;
    }

    xVars = nubBy(_strcmp, allVars(x));
    yVars = nubBy(_strcmp, allVars(y));
    namespace = append(xVars, yVars);
    vs = newVars(len, namespace);

    k = ks->head;
    v = vs->head;

    lamVal *ret;
    for (; ks != NULL; ks = ks->tail, vs = vs->tail) {
        ret = rename_(k, v, ret);
    }

    return ret;
}

lamVal *rename_(char *k, char *v, lamVal *x) {
    char *tmp0;
    lamVal *tmp1, *tmp2;

    switch (x->type) {

        case VAR:
            return strcmp(k, x->varName) == 0 ? newVar(v) : x;

        case ABS:
            tmp0 = strcmp(k, x->absVar) == 0 ? v : x->absVar;
            tmp2 = rename_(k, v, x->absBody);
            return newAbs(tmp0, tmp2);

        case APP:
            tmp1 = rename_(k, v, x->appFunc);
            tmp2 = rename_(k, v, x->appArg);
            return newApp(tmp1, tmp2);
    }

    return NULL;
}

node *boundVars(lamVal *x) {
    return getVars(x, 0, NULL);
}

node *allVars(lamVal *x) {
    return getVars(x, 1, NULL);
}

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

node *newVars(int n, node *excluding) {
    node *ret = NULL;
    size_t bufSize = 1024;
    char buf[bufSize], *tmp;
    memset(&buf, 0, bufSize);
    buf[0] = 'a' - 1;
    int i = 0, j;
    while (n) {
        for (j = i; j > 0 && buf[j] == 'z'; j--) {
            buf[j] = 'a';
        }
        if (buf[j] < 'z') {
            buf[j]++;
        } else {
            buf[j] = 'a';
            buf[++i] = 'a';
        }
        if (!containsBy(_strcmp, buf, excluding)) {
            tmp = malloc(strlen(buf) + 1);
            strcpy(tmp, buf);
            ret = cons(tmp, ret);
            if (!--n) {
                break;
            }
        }
    }
    return ret;
}
