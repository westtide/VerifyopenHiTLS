/*@
    requires 0 < first + second < 180 && 0 < first && first < 180 && 0 < second && second < 180;
    ensures \result == 180 - first -second;
*/
int last_angle(int first, int second){ 
    return 180 - first - second ; 
}