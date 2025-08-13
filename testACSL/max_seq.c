/* max_seq.c */
#include "max_seq.h"

intmax_seq(int* p, int n) {
    int res = *p;
    //@ ghost int e = 0;
    /*@ loop invariant \forall integer j;
                0 <= j < i ==> res >= p[j];
            loop invariant \valid(p + e) && p[e] == res;
            loop invariant about_i: 0 <= i <= n;
            loop invariant 0 <= e < n;
            loop invariant p == \at(p, Pre) && n == \at(n, Pre);
            loop invariant \valid(p + (0..n-1));
        */
    for(int i = 0; i < n; i++) {
        if (res < p[i]) {
            res = p[i];
            //@ ghost e = i;
        }
    }
    return res;
}