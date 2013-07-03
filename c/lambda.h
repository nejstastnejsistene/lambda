#ifndef LAMBDA_H
#define LAMBDA_H

#include "llist.h"

typedef enum { VAR, ABS, APP } lamType;

typedef struct lamVal {
    lamType type;
    union {
        char *varName;   
        struct {
            char *absVar;   
            struct lamVal *absBody;
        };
        struct {
            struct lamVal *appFunc;
            struct lamVal *appArg;
        };
    };
} lamVal;

lamVal *newVar(char*);
lamVal *newAbs(char*, lamVal*);
lamVal *newApp(lamVal*, lamVal*);
lamVal *copyLamVal(lamVal*);
char *showApp(lamVal*, int);
void freeLamVal(lamVal*);

int eq(lamVal*, lamVal*);
int eq_(lamVal*, lamVal*, int, node*, node*);
lamVal *eval(lamVal*);
lamVal *apply(lamVal*, lamVal*);
lamVal *substitute(char*, lamVal*, lamVal*);

char *showLamVal(lamVal*);

#endif
