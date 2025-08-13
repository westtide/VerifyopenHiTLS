/*@
    requires \valid(p) && \valid(q);
    requires -2147483648 <= (integer)(*p) + (integer)(*q )<= 2147483647;
    assigns \nothing;
    ensures \result == *p + *q;
*/
/*
ACSL 1.22 规范 ￼函数合约的顺序应为：
    1.	requires（前置条件）
    2.	terminates（可选）
    3.	decreases（可选）
    4.	assigns / allocates（帧条件）
    5.	ensures（后置条件）
    取值使用：(integer)
    add(&a, &a) 时，p 和 q 指向同一地址，不满足 \separated(p,q)
*/
int add(int *p, int *q) { return *p + *q; }

int main()
{

    int a = 24;
    int b = 42;

    int x;

    x = add(&a, &b);
    //@ assert x == a + b ;
    //@ assert x == 66 ;

    x = add(&a, &a);
    //@ assert x == a + a ;
    //@ assert x == 48 ;
}