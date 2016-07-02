#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double weight[10] = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
double min_weight[10];
int min_id[10];
double temp;
double ans[10000][10];
int main(){
    FILE *fp;
    int k = 0;
    if( (fp=fopen("out3.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf],", &ans[k][0], &ans[k][1]);
                k++;
            }
        }
        fclose(fp);
    }


    for(int i = 0 ; i < 10 ; i ++){
        min_weight[i] = inf;
    }

    for(int i = 0 ; i < k ; i ++){
        for(int j = 0 ; j < 9 ; j ++){
            temp = ans[i][0] * weight[j] + ans[i][1] * (1-weight[j]);
            if(temp < min_weight[j]){
                min_weight[j] = temp;
                min_id[j] = i;
            }
        }
    }
    for(int i = 0 ; i < 9 ; i ++){
        printf("%.1lf %.1lf的最优解是第%d个解,他的值是%lf\n",weight[i],1-weight[i],min_id[i],min_weight[i]);
    }
    return 0;
}
