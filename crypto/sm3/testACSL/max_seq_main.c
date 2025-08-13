/* max_seq_main.c */
#include "max_seq.h"

int main() {
    int array[10] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    int m = max_seq(array, 10);
    //@ assert \exists int i; m == array[i];
    //@ assert \forall int i; 0 <= i < 10 ==> m >= array[i];
    return 0;
}

