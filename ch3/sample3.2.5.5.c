/*  !ensures 只保证了排序，不保证值是原值的排列（permutation）!

    requires \valid(a) && \valid(b) && \valid(c);
    requires \separated(a, b, c);
    assigns *a, *b, *c;
    ensures((integer)(*a) <= (integer)(*b)) && ((integer)(*b) <= (integer)(*c)) && ((integer)(*a) <= (integer)(*c));

*/
/*
    requires \valid(a) && \valid(b) && \valid(c);
    requires \separated(a, b, c);
    assigns *a, *b, *c;
    ensures (integer)(*a) <= (integer)(*b) <= (integer)(*c);
    这也是错的，ACSL不支持三目比较链
    ensures { (integer)\old(*a), (integer)\old(*b), (integer)\old(*c) } ==
            { (integer)(*a), (integer)(*b), (integer)(*c) };
    这也不行，因为ACSL不支持集合的比较
*/

/*@
    requires \valid(a) && \valid(b) && \valid(c);
    requires \separated(a, b, c);
    assigns *a, *b, *c;

    ensures (integer)*a <= (integer)*b && (integer)*b <= (integer)*c;

    // 确保输出值是输入值的排列
    ensures
        \let sa = \old(*a);
        \let sb = \old(*b);
        \let sc = \old(*c);
        \let na = *a;
        \let nb = *b;
        \let nc = *c;
        (na == sa && ((nb == sb && nc == sc) || (nb == sc && nc == sb))) ||
        (na == sb && ((nb == sa && nc == sc) || (nb == sc && nc == sa))) ||
        (na == sc && ((nb == sa && nc == sb) || (nb == sb && nc == sa)));
*/

void order_3(int *a, int *b, int *c)
{
    if (*a > *b)
    {
        int temp = *a;
        *a = *b;
        *b = temp;
    }
    if (*a > *c)
    {
        int temp = *a;
        *a = *c;
        *c = temp;
    }
    if (*b > *c)
    {
        int temp = *b;
        *b = *c;
        *c = temp;
    }
}

void test()
{
    int a1 = 5, b1 = 3, c1 = 4;
    order_3(&a1, &b1, &c1);
    //@ assert a1 == 3 && b1 == 4 && c1 == 5;

    int a2 = 2, b2 = 2, c2 = 2;
    order_3(&a2, &b2, &c2);
    //@ assert a2 == 2 && b2 == 2 && c2 == 2;

    int a3 = 4, b3 = 3, c3 = 4;
    order_3(&a3, &b3, &c3);
    //@ assert a3 == 3 && b3 == 4 && c3 == 4;

    int a4 = 4, b4 = 5, c4 = 4;
    order_3(&a4, &b4, &c4);
    //@ assert a4 == 4 && b4 == 4 && c4 == 5;
}