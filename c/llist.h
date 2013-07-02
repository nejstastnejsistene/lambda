#ifndef LLIST_H
#define LLIST_H

typedef struct node {
    char *head;
    struct node *tail;
} node;

node *cons(char*, node*);
void freeList(node*);
int length(node*);
void append(node*, node*);

int contains(char*, node*);
node *nub(node*);
node *intersection(node*, node*);

#endif
