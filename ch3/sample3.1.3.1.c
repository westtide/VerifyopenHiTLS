/*@ 
    requires -2147483648 <= (integer)x + (integer)y <= 2147483647;
    ensures \result == x + y;
*/
int add(int x, int y){ 
    /*@ assert rte: signed_overflow: -2147483648 ≤ x + y; */
    /*@ assert rte: signed_overflow: x + y ≤ 2147483647; */
    return x+y ; 
}

/**
 * requires -2147483648 <= x <= 2147483647;
 * requires -2147483648 <= y <= 2147483647;
 * ensures -2147483648 <= \old(x)+\old(y) <= 2147483647;
 * 无法从这三条 requires 推导出：-2147483648 <= x + y <= 2147483647
 * 因为即便单独约束了 x 和 y 的范围，两者相加仍可能溢出
 * WP 默认假定 int 是 32 位，但它不会自动推理“这两条单独的 requires 能推出和不溢出”。
 * 因此需要手动添加一个 requires 来确保和不溢出。
 */