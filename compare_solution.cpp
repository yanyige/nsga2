\#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double ans[10000][10];
double ans1[10000][10];
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

    int all = 0;
    for(int i = 0 ; i < k1 ; i ++){
        for(int j = 0 ; j < k ; j ++){
            if(ans[j][0] <= ans[i][0] && ans[j][1] <= ans[i][1]){
                if(ans[i][0] != ans[j][0] || ans[i][1] != ans[j][1]){
                    all ++;
                    break;
                }
            }
        }
    }
    printf("c = %.2lf\n", all / k1);
    return 0;
}

