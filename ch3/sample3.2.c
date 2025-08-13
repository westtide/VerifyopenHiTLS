/*@

    ensures ((a > b) ==> \result == a) && ((a <= b) ==> \result == b);
    ensures \result >= a && \result >= b;
 */
/*
* 在 ACSL 中，==>（蕴含）的优先级低于 &&（逻辑与）。
* 所以需要加括号来明确逻辑顺序。
*/
int max(int a, int b)
{
    return (a > b) ? a : b;
}

void foo()
{

    int a = 42;
    int b = 37;
    int c = max(a, b);

    //@assert c == 42;
}