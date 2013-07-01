#ifndef LLIST_H
#define LLIST_H

typedef struct node {
    void *head;
    struct node *tail;
} node;

node *cons(void*, node*);
void freeList(node*);

node *fromArray(void**, int);
void toArray(node*, void**, int);

node *append(node*, node*);
int length(node*);

typedef int cmp_t(void*, void*);

int containsBy(cmp_t*, void*, node*);
node *nubBy(cmp_t*, node*);
node *intersectBy(cmp_t*, node*, node*);

#endif
