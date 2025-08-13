/*@
    ensures \result <==> (('a'<=c<='z')||('A'<=c<='Z'));

    behavior letter:
        assumes (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        ensures \result == 1;

    behavior non_letter:
        assumes !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
        ensures \result == 0;

    complete behaviors;
    disjoint behaviors;
*/
/* complete
 * 两个行为 letter 和 non_letter 的 assumes 覆盖了所有可能输入（字母或非字母），所以函数调用必定触发某个行为。
 * disjoint：
 * 这两个 assumes 是逻辑互斥的，一个 char 不可能既是字母又是非字母。 
 */
int alphabet_letter(char c){
    if( ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ) 
        return 1;
    else 
        return 0;
}

int main(){
    int r;

    r = alphabet_letter('x');
    //@ assert r;
    r = alphabet_letter('H');
    //@ assert r;
    r = alphabet_letter(' ');
    //@ assert !r;
}