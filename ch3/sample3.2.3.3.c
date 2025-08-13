int h = 42;

/*@
    requires \valid(a) && \valid(b);
    assigns *a, *b;
    ensures *a == \old(*b) && *b == \old(*a);
*/
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main()
{

    int a = 37;
    int b = 91;

    //@ assert h == 42;
    swap(&a, &b);
    //@ assert h == 42;

    return 0;
}

/*@
    requires \valid_read(a);
    requires *a <= 2147483647 - 5;

    assigns \nothing;

    ensures \result == *a + 5;
*/
int plus_5(int *a)
{

    return *a + 5;
}

void test_plus_5()
{
    int x = 10;
    int res = 0;
    res = plus_5(&x);
}