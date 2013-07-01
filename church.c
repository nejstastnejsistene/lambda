#include "lambda.h"

lamVal *toChurch(int x) {
    lamVal *ret = newVar("x");
    while (x--) {
        ret = newApp(newVar("f"), ret);
    }
    return newAbs("f", newAbs("x", ret));;
}
