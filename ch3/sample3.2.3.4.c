#include <limits.h>
/*@
    requires \valid(a) && \valid_read(b);
    requires \separated(a,b);
    requires -2147483648 <= *a + *b <= 2147483647;
    assigns *a;
    ensures *a == \old(*a) + *b;
    ensures *b == \old(*b);
*/
void incr_a_by_b(int *a, int const *b)
{
    *a += *b;
}