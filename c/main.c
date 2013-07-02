#include<stdio.h>

#include "church.h"
#include "lambda.h"

int main() {
    lamVal *zero, *one, *two, *four, *sixteen, *foobar, *foo, *bar;

    zero = toChurch(0);
    one = toChurch(1);
    two = toChurch(2);
    four = toChurch(4);
    foobar = newApp(copyLamVal(two), copyLamVal(four));
    sixteen = apply(two, four);
    bar = apply(sixteen, one);
    
    foo = newApp(newVar("a"), newVar("b"));
    foo = newApp(foo, newVar("c"));
    foo = newApp(foo, newVar("d"));
    foo = newApp(foo, newVar("e"));

    printf("0 => %s\n", showLamVal(zero));
    printf("1 => %s\n", showLamVal(one));
    printf("2 => %s\n", showLamVal(two));
    printf("4 => %s\n", showLamVal(four));
    printf("(2 4) => %s\n", showLamVal(foobar));
    printf("(2 4) => %s\n", showLamVal(sixteen));
    printf("(16 1) => %s\n", showLamVal(bar));
    printf("Left associatives applications:\n");
    printf("(a b c d e) => %s\n", showLamVal(foo));

    freeLamVal(zero);
    freeLamVal(one);
    freeLamVal(two);
    freeLamVal(four);
    freeLamVal(foobar);
    freeLamVal(sixteen);
    freeLamVal(bar);
    freeLamVal(foo);

    return 0;
}
