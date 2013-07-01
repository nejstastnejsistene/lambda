#ifndef ALPHA_H
#define ALPHA_H

#include "lambda.h"
#include "llist.h"

lamVal *alpha(lamVal*, lamVal*);
node *boundVars(lamVal*);
node *allVars(lamVal*);
node *getVars(lamVal*, int, node*);
node *newVars(int, node*);
lamVal *rename_(char*, char*, lamVal*);

#endif
