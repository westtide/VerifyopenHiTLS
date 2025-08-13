#ifndef _MAX
#define _MAX

/*@
    assigns \nothing;

    behavior left_ge:
        assumes x >= y;
        ensures \result == x;

    behavior right_gt:
        assumes x < y;
        ensures \result == y;

    complete behaviors;
    disjoint behaviors;
*/
int max(int x, int y);

#endif