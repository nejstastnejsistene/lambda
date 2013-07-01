#ifndef LLIST_H
#define LLIST_H

typedef struct node {
    char *head;
    struct node *tail;
} node;

node *cons(char*, node*);
void freeList(node*);

node *fromArray(char**, int);
void toArray(node*, char**, int);

node *append(node*, node*);
int length(node*);

int contains(char*, node*);
node *nub(node*);
node *intersection(node*, node*);

#endif
