#include <stdlib.h>

#define NEXT(n, i)  (((n) + (i)/(n)) >> 1)

unsigned int isqrt(int number) {
    unsigned int n  = 1;
    unsigned int n1 = NEXT(n, number);

    while(abs(n1 - n) > 1) {
        n  = n1;
        n1 = NEXT(n, number);
    }
    while(n1*n1 > number) n1--;
    return n1;
}
