#include "utils.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

//Returns the max-valued input variable (e.g max(a,b,c))
int maxValue(int num, int a, int b, int c, int d, int e){
    int nums[5] = {a, b, c, d, e};
    int i;
    int max = 0;

    //Determinar o número max do vetor
    for(i = 0; i < num; i++){
        max = MAX(max, nums[i]);
    }
    return max;
}