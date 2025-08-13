/*@
    requires \valid(q) && \valid(r);
    requires y != 0;
    requires \separated(q,r);
    assigns *q, *r;
    ensures *q == \old(x)/\old(y) && *r == \old(x)%\old(y);
    */
void div_rem(unsigned x, unsigned y, unsigned *q, unsigned *r)
{
    *q = x / y;
    *r = x % y;
}