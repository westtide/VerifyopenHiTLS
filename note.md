# 指针

## `\old` —— 获取函数调用前的值

- **作用**：获取元素在函数调用前（旧状态）的值。
- **使用场景**：仅能在函数的 **后置条件（postcondition）** 中使用。

示例（`swap` 函数）：

```c
/*@
    ensures *a == \old(*b) && *b == \old(*a);
*/
void swap(int* a, int* b);
```
## `\at` —— 获取指定程序点的值

- **语法**：
  - ```\at(表达式, 标签)```
  - 表达式：变量或内存位置
  - 标签：C 语言中的 Label，用于指明程序点

## 内置逻辑标签
- Pre：函数调用前的值，available everywhere
- Old：仅在函数后置条件中可用
- Post：函数返回，仅在函数后置条件中可用
- LoopEntry：循环入口值
- LoopCurrent：当前循环步骤开始时的值
- Here：当前程序点，available everywhere

### `\at`的标签传播与常见错误
例 1：作用域无效
```c
void example_1(void){
  L: ;
  int x = 1;
  //@ assert \at(x, L) == 1; // ❌ 错误，x 在 L 处还不存在
}
```

例 2：未定义值
```c
void example_2(void){
  int x;
  L: x = 1;
  //@ assert \at(x, L) == 1; // ⚠️ 可能无法证明，因为 L 处 x 未初始化
}
```

例 3：指针间接引用
```c
void example_3(void){
  L: ;
  int x = 1;
  int *ptr = &x;
  //@ assert \at(*\at(ptr, Here), L) == 1; // ❌ L 时 ptr 指向的值未定义
}
```
例 4：子表达式传播问题
```c
/*@ requires x + 2 != p; */
void example_4(int* x, int* p){
    *p = 2;

    //@ assert x[2] == \at(x[2], Pre);    // ✅ 可以证明
    //@ assert x[*p] == \at(x[*p], Pre); // ❌ 证明失败
}
// 标签传播规则会使得表达式变成：\at(x[*p], Pre) ≡ \at(x[\at(*p, Pre)], Pre)
```

## 标签的使用场景
例1：函数前后状态比较
```c
/*@ ensures *a == \old(*b) && *b == \old(*a); */
void swap(int* a, int* b);
```
例2：多程序点状态对比
```c
int a = 42;
Label_a:
a = 45;
//@ assert a == 45 && \at(a, Label_a) == 42;  // ✅
```
例3：循环验证（Loop Invariants / Variants）
```c
int sum = 0;
for (int i = 0; i < n; i++) {
    //@ assert \at(sum, LoopEntry) == 0;        // 循环刚开始时
    //@ assert \at(sum, LoopCurrent) <= sum;    // 当前迭代的单调性
    sum += i;
}
```
例4：表达指针或数组在不同状态的值
```c
/*@ requires \valid(a) && \valid(b);
    ensures *a == \old(*b) && *b == \old(*a); */
void swap(int* a, int* b);
```

# 2025-08-08

```c
#define ROUND(A, B, C, D, E, F, G, H, K, FF, GG, Wj, Wi) \
  do                                                     \
  {                                                      \
    uint32_t a12 = ROTL32((A), 12);                      \
    uint32_t ss1 = ROTL32(a12 + (E) + (K), 7);           \
    uint32_t ss2 = ss1 ^ a12;                            \
    uint32_t tt1 = FF((A), (B), (C)) + (D) + ss2 + (Wi); \
    uint32_t tt2 = GG((E), (F), (G)) + (H) + ss1 + (Wj); \
    (H) = tt1;                                           \
    (D) = P0(tt2);                                       \
    (B) = ROTL32((B), 9);                                \
    (F) = ROTL32((F), 19);                               \
  } while (0)

#define ROUND00_15(A, B, C, D, E, F, G, H, K, Wj, Wi) \
  ROUND(A, B, C, D, E, F, G, H, K, FF0, GG0, Wj, Wi)

```
ACSL 里的 logic 函数必须是纯函数，不能写 ```*H = ... ```这种更新。

