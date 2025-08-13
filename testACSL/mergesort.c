/*@
  predicate Sorted(int *t, integer lo, integer hi) =
    \forall integer i, j; lo <= i <= j < hi ==> t[i] <= t[j];
*/

/*@
    requires \valid(A + (0..a- 1));
    requires \valid(B + (0..b - 1));
    requires \valid(res + (0..a + b - 1));
    requires \separated(&A[0..(a-1)], &B[0..(b-1)], &res[0..(a+b-1)]);
    requires a > 0;
    requires b > 0;
    requires Sorted(A, 0, a);
    requires Sorted(B, 0, b);
    assigns res[0..a + b - 1];
    ensures Sorted(res, 0, a + b);
*/
void merge(int A[], int B[], int res[], int a, int b)
{
    int i = 0;
    int j = 0;
    int k = 0;
    /*@
        loop invariant 0 <= i <= a;
        loop invariant 0 <= j <= b;
        loop invariant k == i + j;
        loop invariant 0 <= k <= a + b;
        loop invariant res_sorted_1: Sorted(res, 0, k);
        loop invariant less_than_A: \forall integer p; 0 <= p < k && i < a ==> res[p] <= A[i];
        loop invariant less_than_B: \forall integer p; 0 <= p < k && j < b ==> res[p] <= B[j];
        loop assigns i, j, k;
        loop assigns res[0..a + b - 1];
    loop variant (a + b) - k ;
    */
    while (i < a && j < b)
    {
        if (A[i] <= B[j])
        {
            res[k] = A[i];
            k = k + 1;
            i = i + 1;
        }
        else
        {
            res[k] = B[j];
            k = k + 1;
            j = j + 1;
        }
    }
    //@ ghost End_l: ;

    /*@
        loop invariant 0 <= i <= a;
        loop invariant k == i + j;
        loop invariant \at(k, End_l) <= k <= a + b;
        loop invariant res_sorted_2: Sorted(res, 0, k);
        loop invariant less_than_A_2: \forall integer p; 0 <= p < k && i < a ==> res[p] <= A[i];
    loop invariant unchanged_beg: \forall integer p; 0 <= p < \at(k, End_l) ==> res[p] == \at(res[p], End_l);
    loop invariant unchanged_end: \forall integer p; k <= p < a+b ==> res[p] == \at(res[p], End_l);
        loop assigns res[0..a + b - 1];
        loop assigns i, k;
    loop variant a - i ;
    */
    while (i < a)
    {
        res[k] = A[i];
        k = k + 1;
        i = i + 1;
    }

    /*@
        loop invariant 0 <= j <= b;
        loop invariant k == i + j;
        loop invariant 0 <= k <= a + b;
        loop invariant res_sorted_3: Sorted(res, 0, k);
        loop invariant less_than_B_2:\forall integer p; 0 <= p < k && j < b ==> res[p] <= B[j];
        loop assigns res[0..a + b - 1];
        loop assigns j, k;
    loop variant b - j;
    */
    while (j < b)
    {
        res[k] = B[j];
        k = k + 1;
        j = j + 1;
    }
}