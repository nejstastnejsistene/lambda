#include<stdlib.h>
#include<string.h>

#include "llist.h"

// Create a new node from a head and a tail.
node *cons(char *head, node *tail) {
    node *x = malloc(sizeof(node));  
    x->head = head;
    x->tail = tail;
    return x;
}

/*
// Convert an array of strings into a list of strings.
node *fromArray(char **arr, int len) {
    node *x = NULL;
    while (len--) {
        x = cons(arr[len], x);
    }
    return x;
}

// Create a copy of list. Oonly the nodes are copies, the contents of the
// the list remain unmodified.
node *copyList(node *x) {
    node *rev, *ret;
    // Elegant, but somewhat wasteful.
    rev = reverse(x);
    ret = reverse(rev);
    freeList(rev);
    return ret;
}

// Reverse a list.
node *reverse(node *x) {
    node *y = NULL;
    for (; x != NULL; x = x->tail) {
        y = cons(x->head, y);
    }
    return y;
}

// Copy a list of strings into an array of strings.
void toArray(node *x, char **arr, int len) {
    arr += len;
    for (arr += len; len--; x = x->tail) {
        *(--arr) = x->head;
    }
}
*/

// Free all of the nodes in a list. The contents of the list are not freed.
void freeList(node *x) {
    node *prev;
    while (x != NULL) {
        prev = x;
        x = x->tail;
        //free(prev->head);
        free(prev);
    }
}

// Return the length of a list.
int length(node *x) {
    int n = 0;
    for (; x != NULL; x = x->tail) n++;
    return n;
}

// Append the second list to the first list. The first list can not be null.
void append(node *x, node *y) {
    node *tmp;

    // Iterate to the end of a copy of the first list.
    for (tmp = x; tmp->tail != NULL; tmp = tmp->tail); 

    // Link lists together.
    tmp->tail = y;
}

// Return whether val is contained within list.
int contains(char *val, node *list) {
    for (; list != NULL; list = list->tail) {
        if (strcmp(list->head, val) == 0) {
            return 1;
        }
    }
    return 0;
}

// Remove all duplicates from a list.
void nub(node **x) {
    node *p, *set = NULL;

    for (p = *x; p != NULL; p = p->tail) {
        // Insert the item if it is not already in ret.
        if (!contains(p->head, set)) {
            set = cons(p->head, set);
        }
    }
    freeList(*x);
    *x = set;
}

// Return the intersection of two lists. Assumes that there are no duplicates.
node *intersection(node *x, node *y) {
    node *ret = NULL;
    for (; x != NULL; x = x->tail) {
        // Insert the item if it is also in the second list.
        if (contains(x->head, y)) {
            ret = cons(x->head, ret);
        }
    }
    return ret;
}
