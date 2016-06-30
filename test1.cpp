#include<stdio.h>

double test[14][14];
int main(){
    freopen("in2.txt", "r", stdin);
    freopen("out2.txt", "w", stdout);
    for(int i = 0 ; i < 14 ; i ++){
        for(int j = 0 ; j < 14 ; j ++){
            scanf("%lf", &test[i][j]);
        }
    }

    for(int i = 0 ; i < 14 ; i ++){
        for(int j = 0 ; j < 14
         ; j ++){
            printf("%lf ", test[i%14][j%14]);
        }
    }


    return 0;
}

