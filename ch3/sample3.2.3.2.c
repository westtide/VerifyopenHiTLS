/*@ 
    requires \valid(a) && \valid(b);
    ensures *a == \old(*b) && *b == \old(*a);

*/ 
void swap(int* a, int* b){ 
    int tmp = *a; 
    *a = *b; 
    *b = tmp; 
}

/*@ requires \valid_read(p); */ 
/*
解引用p是有效的，然而WP没有验证unref的前置条件，
因为解引用的值仅适用于读访问。写访问将导致未定义的行为。
在这种情况下，我们可以指定指针p必须是\valid_read而不是\valid。
*/
int unref(int* p){ 
    return *p; 
}

int const value = 42; // 只读

int main(){ 
    int i = unref(&value); 
}

