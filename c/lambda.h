#ifndef LAMBDA_H
#define LAMBDA_H

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

char *copyString(char*);
lamVal *newVar(char*);
lamVal *newAbs(char*, lamVal*);
lamVal *newApp(lamVal*, lamVal*);
lamVal *copyLamVal(lamVal*);
void freeLamVal(lamVal*);

lamVal *apply(lamVal*, lamVal*);
lamVal *substitute(char*, lamVal*, lamVal*);

char *showLamVal(lamVal*);

#endif
