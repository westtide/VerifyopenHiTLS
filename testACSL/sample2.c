#include <limits.h>

/*@ requires INT_MIN < val;
    ensures \result >= 0;
    ensures (val >= 0 ==> \result == val) &&
            (val < 0 ==> \result == -val);
*/
int abs(int val) {
    if (val < 0)
        return -val;
    return val;
}

// 验证函数调用是否满足前置条件
void foo1(int a){
    int b = abs(42);          // OK
    int c = abs(-42);         // OK
    int d = abs(a);           // 可能不满足要求：a 可能等于 INT_MIN
    int e = abs(INT_MIN);     // 明确不满足：val == INT_MIN 不满足 precondition
}

// 验证顺序影响推理
void foo2(int a){
    int e = abs(INT_MIN);     //  不满足前置条件，断定为 false
    int d = abs(a);           //  居然被证明了？
  /* 
    在 deductive verification（演绎验证）中，
    一旦某个断言（如 abs(INT_MIN)）违背了其前置条件，
    即导致整个执行路径“进入了错误态”。
    这意味着 Frama-C 推理系统现在知道了一件“不可能的事发生了”——
    即一个逻辑上的“假（false）”。
    逻辑上，如果我们知道某个假命题为真（即 false 成立），
    那么我们就可以推出任意命题为真（称为 ex falso quodlibet）。
    因此，在接下来的调用 abs(a) 中，F
    rama-C 的证明器将 false 纳入其假设前提中，
    于是所有属性都可以“证明”为成立。
 */
}
