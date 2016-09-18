#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<algorithm>
#include<vector>
#include<math.h>
#include<set>
#include<stack>

using namespace std;

const int MAXN = 2000;
const int number_of_tasks = 16;
const int number_of_machines =2;//改机器时要改实例8,14
const int m = number_of_machines;
const int n = number_of_tasks;
const int inf = 0x3f3f3f3f;
const int cycle=3;//周期数
//*******
//**pop - Population size
//**gen - Total number of generations
int pop, gen;
int isdep[cycle][n];//被i依赖的个数
int todep[cycle][n];//i依赖的任务数
int taskIndex[MAXN];// 记录每个机器所匹配到的位置
bool taskUsed[n];
set<int> doneSet;

/****************************************
目标函数   Minimize communication cost
目标函数   Minimize the balance of processor workload
t(m,n)     Execution time of a task on a processor
c(m,n)     Communication cost between two tasks
****************************************/


double t[m][n];
struct time_table
{
    double time;
    int id;
} time_in_machine[n];

double c[n][n];
bool uesd[MAXN];

struct Individual
{
    vector<int> machine[number_of_machines]; //save permutation of the tasks
    double workload; //object 1
    double makespan; //object 2
    int front; //rank of domination
    vector< int > S; //the collections dominated by this Individual
    int n;// count of dominated solution
    double dfitness; //fitness
    double crowd_distance;
} Collection[MAXN], new_individual;

vector<Individual> Front[MAXN];

void solve()
{
    int depend = 0;
    scanf("%d%d", &pop, &gen);
    //所有任务在不同机器上执行时间相同
    for(int i=0; i<n; i++)
    {
        scanf("%lf",&t[0][i]);
    }
    for(int j=1; j<m; j++)
    {
        for(int i=0; i<n; i++)
        {
            t[j][i]=t[0][i];
        }
    }

    for(int i = 0 ; i < n ; i ++)
    {
        for(int j = 0 ; j < n ; j ++)
        {
            scanf("%lf", &c[i][j]);
            if(c[i][j] > 0) {
                depend ++;
            }
        }
    }

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<cycle; j++)
        {
            isdep[j][i]=0;//依赖任务i的个数
            todep[j][i]=0;//i依赖的任务数
        }

    }
    for(int i = 0 ; i < n ; i ++)
    {
        for(int j = 0 ; j < n ; j ++)
        {
            if(c[i][j]>0)
            {
                todep[0][i]++;
                isdep[0][j]++;
            }
        }
    }
    for(int i=1; i<cycle; i++)
    {
        for(int j=0; j<n; j++)
        {
            todep[i][j]=todep[0][j];
            isdep[i][j]=isdep[0][j];
        }
    }
    printf("depend = %d\n", depend);
}

//void get_segment_array(Individual *i, int **)
int main()
{
    srand(4);
    freopen("MP3_decoder_16.dat", "r", stdin);
    freopen("depend.txt", "w", stdout);
    solve();

    return 0;
}
