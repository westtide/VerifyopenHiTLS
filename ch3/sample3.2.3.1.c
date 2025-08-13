/*@
    ensures *a == \old(*b) && *b == \old(*a);
*/
void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int function_1() 
{
    int a = 42; 
Label_a:
    a = 45;
    //@ assert a == 45 && \at(a, Label_a) == 42;
}

/*@ 
requires \valid(a) ^ \valid(b); 
ensures *\old(a) == \old(*b) ^ *\old(b) == \old(*a); 
*/ 
void swap2(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}