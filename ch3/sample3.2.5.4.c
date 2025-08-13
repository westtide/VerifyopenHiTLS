/*@
    requires \valid(a) && \valid(b);
    requires -2147483648 <= (integer)(*a) <= 2147483647;
    requires -2147483648 <= (integer)(*b) <= 2147483647;
    assigns \nothing;
    ensures \result == ((integer)(*a) < (integer)(*b) ? (integer)(*b) : (integer)(*a));
*/
int max_ptr(int *a, int *b)
{
    return (*a < *b) ? *b : *a;
}

/*@
    requires \valid(a) && \valid(b);
    assigns \nothing;
*/
void max_ptr2(int *a, int *b);

extern int h;

int main()
{
    h = 42;

    int a = 24;
    int b = 42;

    int x = max_ptr(&a, &b);
    max_ptr2(&a, &b);

    //@ assert x == 42 ;
    //@ assert h == 42 ;
}