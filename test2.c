/*@ predicate valid_str{L}(char *s) =
      \valid(s) && \forall integer i; 0 <= i < strlen(s) ==> \valid(s+i);
*/

/*@ logic char *strchrnul(char *s, char c) =
      (*s == c) ? s :
      (*s == '\0') ? s :
      strchrnul(s + 1, c); 
*/

/*@ ghost
  @ /@ lemma
  @ @ requires valid_str(s);
  @ @ decreases strlen(s);
  @ @ ensures s <= strchrnul(s, c) <= s + strlen(s);
  @*/
void strchrnul_in_range(char *s, char c) {
    if (*s != '\0' && *s != c)
        strchrnul_in_range(s + 1, c);
}

/*@ axiomatic LF__Axiomatic__strchrnul_in_range {
  axiom LF__Lemma__strchrnul_in_range:
    \forall char *s, char c; valid_str(s) ==>
      s <= strchrnul(s, c) <= s + strlen(s);
} */

/*@ predicate LF__Pred__strchrnul_in_range(Z x) = \true; */

/*@ requires n >= 0 && \valid(t + (0 .. n-1));
  @ assigns \nothing;
  @ ensures -1 <= \result < n;
  @ behavior success:
  @   assumes sorted(t,0,n-1);
  @   ensures \result >= 0 ==> t[\result] == v;
  @ behavior failure:
  @   assumes sorted(t,0,n-1);
  @   ensures \result == -1 ==>
  @     \forall integer k; 0 <= k < n ==> t[k] != v;
  @ complete behaviors success, failure;
  @ disjoint behaviors success, failure;
*/
int binary_search(long t[], int n, long v) {
  int l = 0, u = n-1;
  /*@ loop invariant 0 <= l && u <= n-1;
    @ loop invariant \forall integer k; 0 <= k < n && t[k] == v ==> l <= k <= u;
    @ loop assigns l, u;
    @ loop variant u - l;
  */
  while (l <= u) {
    int m = (l + u) / 2;
    if (t[m] < v) l = m + 1;
    else if (t[m] > v) u = m - 1;
    else return m;
  }
  return -1;
}
