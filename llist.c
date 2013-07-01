#include<stdlib.h>
#include<stdio.h>

#include "llist.h"

node *cons(void *head, node *tail) {
    node *x = malloc(sizeof(node));  
    x->head = head;
    x->tail = tail;
    return x;
}

node *reverse(node *x) {
    node *y = NULL;
    for (; x != NULL; x = x->tail) {
        y = cons(x->head, y);
    }
    return y;
}

node *copyList(node *x) {
    node *rev, *ret;
    rev = reverse(x);
    ret = reverse(rev);
    freeList(rev);
    return ret;

}

void freeList(node *x) {
    node *prev;
    while (x != NULL) {
        prev = x;
        x = x->tail;
        free(prev->head);
        free(prev);
    }
}

int length(node *x) {
    int n = 0;
    for (; x != NULL; x = x->tail) n++;
    return n;
}

node *fromArray(void **arr, int len) {
    node *x = NULL;
    while (len--) {
        x = cons(arr + len, x);
    }
    return x;
}

void toArray(node *x, void **arr, int len) {
    arr += len;
    for (arr += len; len--; x = x->tail) {
        *(--arr) = x->head;
    }
}

node *append(node *x, node *y) {
    if (x == NULL) {
        return y;
    }
    node *ret, *tmp;
    ret = tmp = copyList(x);
    while (tmp->tail != NULL) {
        tmp = tmp->tail;
    }
    tmp->tail = y;
    return ret;
}

int containsBy(cmp_t *cmp, void *val, node *list) {
    for (; list != NULL; list = list->tail) {
        if (cmp(list->head, val) == 0) {
            return 1;
        }
    }
    return 0;
}

node *nubBy(cmp_t *cmp, node *x) {
    node *y = NULL;
    for (; x != NULL; x = x->tail) {
        if (!containsBy(cmp, x->head, y)) {
            y = cons(x->head, y);
        }
    }
    return y;
}

node *intersectBy(cmp_t *cmp, node *x, node *y) {
    node *z = NULL;
    for (; x != NULL; x = x->tail) {
        if (containsBy(cmp, x->head, y)) {
            z = cons(x->head, z);
        }
    }
    return z;
}
