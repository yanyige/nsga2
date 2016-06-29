#include<stdio.h>

double test[14];
int main(){
    freopen("in1.txt", "r", stdin);
    freopen("out1.txt", "w", stdout);
    for(int i = 0 ; i < 14 ; i ++){
        scanf("%lf", &test[i]);
    }

    for(int i = 0 ; i < 8 ; i ++){
        for(int j = 0 ; j < 14 ; j ++){
            printf("%lf ", test[j%14]);
        }
    }


    return 0;
}
