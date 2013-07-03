#ifndef LLIST_H
#define LLIST_H

typedef struct node {
    char *head;
    int val;
    struct node *tail;
} node;

node *cons(char*, node*);

// dict functions...
node *put(char*, int, node*);
int lookup(char*, node*);

void freeList(node*);
int length(node*);
void append(node*, node*);

int contains(char*, node*);
void nub(node**);
node *intersection(node*, node*);

#endif
