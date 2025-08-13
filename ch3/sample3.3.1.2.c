// First, a function that returns if a character is a vowel or a consonant, one should assume (and express) that the function receives a lowercase letter.

enum Kind
{
    VOWEL,
    CONSONANT
};

/*@
    requires ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    assigns \nothing;
    behavior v:
        assumes c \in {'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'};
        ensures \result == VOWEL;
    behavior c:
        assumes c \in {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z'};
        ensures \result == CONSONANT;
    complete behaviors;
    disjoint behaviors;
*/
enum Kind kind_of_letter(char c)
{
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
    {
        return VOWEL;
    }
    else
    {
        return CONSONANT;
    }
}

/*@
    assigns \nothing;
    behavior case1:
        assumes x > 0 && y > 0;
        ensures \result == 1;
    behavior case2:
        assumes x < 0 && y > 0;
        ensures \result == 2;
    behavior case3:
        assumes x < 0 && y < 0;
        ensures \result == 3;
    behavior case4:
        assumes x > 0 && y < 0;
        ensures \result == 4;
    behavior case5:
        assumes x == 0 && y != 0;
        ensures \result == 5;
    behavior case6:
        assumes y == 0 && x != 0;
        ensures \result == 6;
    behavior case7:
        assumes x == 0 && y == 0;
        ensures \result == 7;
    complete behaviors;
    disjoint behaviors;
*/
int quadrant(int x, int y)
{
    if (x > 0 && y > 0)
    {
        return 1; // 第一象限
    }
    else if (x < 0 && y > 0)
    {
        return 2; // 第二象限
    }
    else if (x < 0 && y < 0)
    {
        return 3; // 第三象限
    }
    else if (x > 0 && y < 0)
    {
        return 4; // 第四象限 (x > 0 && y < 0)
    }
    else if (x == 0 && y != 0)
    {
        return 5; // y轴上 (x == 0 && y != 0)
    }
    else if (y == 0 && x != 0)
    {
        return 6; // x轴上 (y == 0 && x != 0)
    }
    else
    {
        return 7; // 原点 (x == 0 && y == 0)
    }
}

int main()
{
    char c = 'a';
    char type = (kind_of_letter(c) == VOWEL) ? 'v' : 'c';
    //@ assert type == 'v';

    int x = 3, y = 4;
    int q = quadrant(x, y);
    //@ assert q == 1;

    return 0;
}