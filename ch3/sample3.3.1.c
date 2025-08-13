#include <limits.h>

/*@
    requires a < b ==> b - a <= INT_MAX ;
    requires b <= a ==> a - b <= INT_MAX ;
    assigns \nothing;
    behavior pos:
        assumes a < b;
        ensures \result == b - a;
    behavior neg:
        assumes a >= b;
        ensures \result == a - b;
    complete behaviors;
    disjoint behaviors;
*/
int distance(int a, int b)
{
    if (a < b)
        return b - a;
    else
        return a - b;
}

/*@
    requires \valid(a) && \valid_read(b) ;
    requires \separated(a, b) ;
    assigns *a;
    behavior pos:
        assumes (integer)(*b) != 0;
        ensures (integer)(*a) == 0;
    behavior neg:
        assumes (integer)(*b) == 0;
        ensures (integer)(*a) == \old(*a);
    complete behaviors;
    disjoint behaviors;

*/
void reset_1st_if_2nd_is_true(int *a, int const *b)
{
    if (*b)
        *a = 0;
}

/*@
    requires 1 <= m <= 12 ;
    assigns \nothing;
    behavior case1:
        assumes m \in { 2 };
        ensures \result == 28 ;
    ensures m \in { 2 } ==> \result == 28 ;
    behavior case2:
        assumes m \in { 1, 3, 5, 7, 8, 10, 12 };
        ensures \result == 31 ;
    behavior case3:
        assumes m \in { 4, 6, 9, 11 };
        ensures \result == 30 ;
    complete behaviors;
    disjoint behaviors;
*/
int day_of(int m)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[m - 1];
}

/*@
    requires \valid(a) && \valid(b);
    assigns *a, *b ;
    behavior pos:
        assumes  *a < *b ;
        ensures  *a == \old(*b) && *b == \old(*a) ;
    behavior neg:
        assumes *a >= *b;
        ensures *a == \old(*a) && *b == \old(*b) ;
    complete behaviors;
    disjoint behaviors;
*/
/*
    •	\old 只能在 后置条件（ensures、assigns、terminates）中使用
    •	不能在 requires 或 assumes 中使用
    */
void max_ptr(int *a, int *b)
{
    if (*a < *b)
    {
        int tmp = *b;
        *b = *a;
        *a = tmp;
    }
}