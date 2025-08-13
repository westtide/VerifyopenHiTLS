/*@
    requires (-2147483648 <= (integer)a - (integer)b <= 2147483647) &&
             (-2147483648 <= (integer)b - (integer)a <= 2147483647);
    ensures (a < b ==> \result == \old(b) - \old(a)) &&
            (a >= b ==> \result == \old(a) - \old(b)) &&
            (\result >= 0);
*/
int distance(int a, int b){ 
    if(a < b) 
        return b - a ; 
    else 
        return a - b ; 
}