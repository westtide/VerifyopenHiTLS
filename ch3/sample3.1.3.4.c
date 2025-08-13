/*@
    requires 1 <= month <= 12;
    ensures ((month == 1)||(month == 3)||(month == 5)||(month == 7)||
                (month == 8)||(month == 10)||(month == 12)) ==> \result == 31 &&
            ((month == 4)||(month == 6)||(month == 9)||(month == 11)) ==> \result == 30 &&
            (month == 2) ==> \result == 28;
    ensures (month \in {1,3,5,7,8,10,12} ==> \result == 31) &&
            (month \in {4,6,9,11} ==> \result == 30) &&
            (month == 2 ==> \result == 28);

 */
int day_of(int month){ 
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } ; 
    return days[month-1] ; 
}