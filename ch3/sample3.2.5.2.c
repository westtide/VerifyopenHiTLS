
/*@
    requires \valid(a) && \valid(b);
    assigns *a;
    ensures *b != 0 ==> *a == 0;
    ensures *b == 0 ==> *a == \old(*a);
 */
/*
    Frama-C WP 插件的自动证明是基于函数契约的，不会直接依赖函数体去推理值不变（除非内联展开）。
    断言 //@ assert a == 5; 依赖于知道当 *b == 0 时 *a 不被修改。
    但是你没有在 ensures 中明确说明这一点，因此证明器只能做部分假设（Valid_under_hyp）。
    */
void reset_1st_if_2nd_is_true(int *a, int const *b)
{
    if (*b)
        *a = 0;
}

int main()
{

    int a = 5;
    int x = 0;

    reset_1st_if_2nd_is_true(&a, &x);
    //@ assert a == 5 ;
    //@ assert x == 0 ;

    int const b = 1;
    reset_1st_if_2nd_is_true(&a, &b);
    //@ assert a == 0 ;
    //@ assert b == 1 ;
}