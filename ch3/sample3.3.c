#include <limits.h>

/*@

    requires val > INT_MIN; assigns \nothing;
    ensures \result >= 0;
    behavior pos:
        assumes 0 <= val; ensures \result == val;
    behavior neg:
        assumes val < 0; ensures \result == -val;
    complete behaviors;
    disjoint behaviors;
*/

/*
    •	disjoint behaviors（互斥行为）:对于任何输入，最多匹配一个行为。
    •	complete behaviors（完备行为）:对于任何输入，至少匹配一个行为。
    •	行为声明并不禁止全局 postcondition
    •   目前 WP 并不擅长处理每个行为单独定义的 assigns,建议统一写一个全局 assigns，然后在行为的 ensures 中指出哪些元素实际上未修改（x == \old(x)）

*/
int abs(int val)
{
    if (val < 0)
        return -val;
    return val;
}