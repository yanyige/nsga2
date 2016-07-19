#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double ans[10000][10];
double ans1[10000][10];
int main(){
    FILE *fp;
    int k = 0;
    if( (fp=fopen("out1.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf],", &ans[k][0], &ans[k][1]);
                k++;
            }
        }
        fclose(fp);
    }
    int k1 = 0;
    if( (fp=fopen("out2.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf],", &ans1[k1][0], &ans1[k1][1]);
                k1++;
            }
        }
        fclose(fp);
    }
    for(int i = 0 ; i < k ; i ++){
        printf("i = %d:", i);
        for(int j = 0 ; j < 2 ; j ++){
            printf("%.2lf ", ans[i][j]);
        }
        printf("\n");
    }
    for(int i = 0 ; i < k1 ; i ++){
        printf("i = %d:", i);
        for(int j = 0 ; j < 2 ; j ++){
            printf("%.2lf ", ans1[i][j]);
        }
        printf("\n");
    }

    int all = 0;
    for(int i = 0 ; i < k1 ; i ++){
        for(int j = 0 ; j < k ; j ++){
            if(ans[j][0] <= ans1[i][0] && ans[j][1] <= ans1[i][1]){
                if(ans1[i][0] != ans[j][0] || ans1[i][1] != ans[j][1]){
                    all ++;
                    printf("%dÖ§Åä%d\n", j , i);
                    break;
                }
            }
        }
    }
    printf("all = %d k1 = %d\n", all, k1);
    double ret = all / k1;
    printf("c = %.2lf\n", ret);
    return 0;
}

