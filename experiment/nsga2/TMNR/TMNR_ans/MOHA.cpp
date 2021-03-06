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
const int number_of_tasks = 14;
const int number_of_machines =4;//改机器时要改实例8,14
const int m = number_of_machines;
const int n = number_of_tasks;
const int inf = 0x3f3f3f3f;
const int cycle=2;//周期数
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

int cmp(const void *a, const void *b)
{
    return (*(Individual *)a).workload > (*(Individual *)b).workload ? 1:-1;
}

int cmp1(const void *a, const void *b)
{
    return (*(Individual *)a).makespan > (*(Individual *)b).makespan ? 1:-1;
}

int cmp2(const void *a, const void *b)
{
    return (*(Individual *)a).crowd_distance > (*(Individual *)b).crowd_distance ? -1:1;
}

int cmp3(const void *a, const void *b)
{
    return (*(time_table *)a).time > (*(time_table *)b).time ? -1 : 1;
}

void copy_individual(Individual *i, Individual *j)
{
    vector<int>:: iterator iter;
    for(int ii = 0 ; ii < m ; ii ++)
    {
        i->machine[ii].erase(i->machine[ii].begin(), i->machine[ii].end());
    }
    for(int ii = 0 ; ii < m ; ii ++)
    {
        for(iter = j->machine[ii].begin(); iter != j->machine[ii].end(); ++ iter)
        {
            i->machine[ii].push_back((*iter));
        }
    }
}

double abs(double t)
{
    return t>0?t:-t;
}

void evaluate_objective(Individual *i)
{
    vector<int>:: iterator iter;
    vector<int>:: iterator jter;

//    printf("evaluate的结果\n");
//    for(int j = 0 ; j < m ; j ++){
//        printf("第%d个机器: ", j);
//        for(iter = i->machine[j].begin(); iter != i->machine[j].end() ; iter ++){
//            printf("%d ", (*iter));
//        }
//        printf("\n");
//    }


    i->n = inf;
    i->makespan = 0;
    i->workload = 0;
    double depcopy[cycle][n][n];
    int isdepcopy[cycle][n],todepcopy[cycle][n];
    for(int k=0; k<cycle; k++)
    {
        for(int ii=0; ii<n; ii++)
        {
            isdepcopy[k][ii]=isdep[k][ii];
            todepcopy[k][ii]=todep[k][ii];
            for(int j=0; j<n; j++)
            {

                depcopy[k][ii][j]=c[ii][j];

            }

            // printf("%lf ",c[ii][j]);
        }
        //printf("\n");
    }
    double load[m];
    memset(load, 0, sizeof(load));
    int jud_index[m],flag_end[m],position;
    memset(jud_index,0,sizeof(jud_index));
    memset(flag_end,0,sizeof(flag_end));
    double E[m];
    memset(E,0,sizeof(E));
    double tao[cycle][n];
    double taoend[cycle][n];
    double Lstart,ltemp;
    int it=1,task_in_machine[n],flag;
    double avg = 0,workload=0;
    for(int j = 0 ; j < n ; j ++)
    {
        avg += t[0][j];
    }
    avg /= m;
    for(int j = 0 ; j < number_of_machines ; j ++)
    {
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); ++ iter)
        {
            task_in_machine[*iter]=j;
            load[j]=load[j]+t[j][(*iter)];
            //printf("%d:%d ",*iter,task_in_machine[*iter]);
        }
//        printf("load[%d]=%lf\n",j,load[j]);
        workload+= abs(load[j]-avg);
    }
    i->workload=workload;
    while(it<=n*cycle)
    {
        //printf("iter=%d\n",it);
//        for(int j=0;j<n;j++){
//            tao[cyc][n]=0;
//        }
//    }
        //for(int cyc=0;cyc<cycle;cyc++){
        for(int ii=0; ii<m; ii++)
        {
            if(flag_end[ii]==cycle||i->machine[ii].empty()) {
                continue;
            }
            position=i->machine[ii].at(jud_index[ii]);
            //printf("this %d time:%d ",ii,position);
            //exit(0);
            if(isdepcopy[flag_end[ii]][position]==0)
            {
                //printf("here\n");
                Lstart=E[ii];
                for(int kk=0; kk<n; kk++)
                {
                    if((c[kk][position]>0)&&(task_in_machine[kk]!=task_in_machine[position]))
                    {
                        ltemp=taoend[flag_end[ii]][kk]+c[kk][position]+2;
                        if(ltemp>Lstart)
                        {
                            Lstart=ltemp;
                        }
                    }
                }

                tao[flag_end[ii]][position]=Lstart;
                E[ii]=tao[flag_end[ii]][position]+t[ii][position];
                taoend[flag_end[ii]][position]=E[ii];
//                Lstart=0;
//                for(int kk=0; kk<n; kk++)
//                {
//                    if((c[position][kk]>0)&&(task_in_machine[kk]!=task_in_machine[position]))
//                    {
//                        ltemp=c[position][kk];
//                        if(ltemp>Lstart)
//                        {
//                            Lstart=ltemp;
//                        }
//                    }
//                }
               // E[ii]+=Lstart;
               for(int kk=0;kk<n;kk++){
                    if(c[position][kk]>0 && (task_in_machine[kk] != task_in_machine[position])){
                        E[ii]+=2;
                        break;
                    }
               }
                it++;
                for(int jj=0; jj<n; jj++)
                {
                    // printf("decopy[%d][%d]=%lf ",position,jj,depcopy[position][jj]);
                    if(depcopy[flag_end[ii]][position][jj]>0&&position!=jj)
                    {

                        depcopy[flag_end[ii]][position][jj]=0.0;
                        isdepcopy[flag_end[ii]][jj]--;
                        //printf("is[%d]=%d\n",jj,isdep[jj]);
                        todepcopy[flag_end[ii]][position]--;
                    }
                }
                //printf("end:%d ",*(i->machine[ii].rbegin()));
                if(position==(*(i->machine[ii].rbegin()))) //
                {
                    flag_end[ii]++;
                    jud_index[ii]=0;
                    //printf("flag=%d\n",flag_end[ii]);
                }
                else
                {
                    jud_index[ii]++;
                }

            }
            //  }

        }
        //printf("\n");
        //  }
    }
//    for(int ii=0; ii<cycle; ii++)
//    {
//        for(int jj=0; jj<n; jj++)
//        {
//            printf("T[%d][%d]=%lf\n",ii,jj,tao[ii][jj]);
//        }
//    }


    for(int jj=0; jj<m; jj++)
    {
//        printf("E[%d]=%lf\n",jj,E[jj]);
        if(E[jj]>i->makespan)
        {
            i->makespan=E[jj];
        }
    }
//    printf("workload=%lf,makespan=%lf\n",i->workload,i->makespan);
//    printf("************************************\n");
//    double span = 0;
//    for(int j = 0 ; j < number_of_machines ; j ++){
//        double tspan = 0;
//        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); ++ iter){
//            tspan += t[j][(*iter)];
//            for(int jj = 0 ; jj < number_of_machines ; jj ++){
//                if(jj != j){
//                    for(jter = i->machine[jj].begin(); jter != i->machine[jj].end(); ++ jter){
//                        i->communication_cost += c[(*iter)][(*jter)];
//                    }
//                }
//            }
//        }
//        span += abs(tspan - avg);
//    }
//    i->maxspan = span;

//    for(int j = 0 ; j < number_of_machines ; j ++){
//        double tspan = 0;
//        for(vector<int>::iterator iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end(); ++ iter){
//            tspan += t[j][(*iter)];
//            for(int jj = 0 ; jj < number_of_machines ; jj ++){
//                if(jj != j){
//                    for(vector<int>::iterator jter = Collection[1].machine[jj].begin(); jter != Collection[1].machine[jj].end(); ++ jter){
//                        Collection[1].communication_cost += c[(*iter)][(*jter)];
//                    }
//                }
//            }
//        }
//        printf("tspan = %.2lf\n", tspan);
//        span += abs(tspan - avg);
//    }
//    i->maxspan = span;
}

// 非支配排序
void non_domination_sort(Individual individuals[], int length, bool last)
{
    vector< Individual > frontCollection;
    vector< Individual > tempCollection;
    int rank = 1;
    for(int i = 0 ; i < length ; i ++)
    {
        //initial
        individuals[i].S.erase(individuals[i].S.begin(), individuals[i].S.end());
        individuals[i].n = 0;
        for(int j = 0 ; j < length ; j ++)
        {
            if(i!=j)
            {
                if(!last) {
                    // if individual[i] dominate individual[j]
                    if(individuals[i].makespan < individuals[j].makespan && individuals[i].workload < individuals[j].workload)
                    {
                        // let individual[j] added to the S of the individual[i]
                        individuals[i].S.push_back(j);
                    }
                    else if(individuals[j].makespan < individuals[i].makespan && individuals[j].workload < individuals[i].workload)
                    {
                        individuals[i].n = individuals[i].n + 1;
                    }
                }
                else {
                    // if individual[i] dominate individual[j]
                    if(individuals[i].makespan <= individuals[j].makespan && individuals[i].workload <= individuals[j].workload){
                        // let individual[j] added to the S of the individual[i]
                        if(individuals[i].makespan == individuals[j].makespan && individuals[i].workload == individuals[j].workload){
                            continue;
                        }
                        individuals[i].S.push_back(j);

                    }else if(individuals[j].makespan <= individuals[i].makespan && individuals[j].workload <= individuals[i].workload){
                        if(individuals[i].makespan == individuals[j].makespan && individuals[i].workload == individuals[j].workload){
                            continue;
                        }
                        individuals[i].n = individuals[i].n + 1;
                    }
                }
            }
        }
    }
    for(int i = 0 ; i < MAXN ; i ++)
    {
        vector<Individual>().swap(Front[i]);
    }
    for(int i = 0 ; i < length ; i ++)
    {
        if(individuals[i].n == 0)
        {
            individuals[i].front = rank;
            frontCollection.push_back(individuals[i]);
            Front[rank].push_back(individuals[i]);
        }
    }

    while(!frontCollection.empty())
    {
        tempCollection.erase(tempCollection.begin(), tempCollection.end());
        for(vector<Individual>::iterator iter = frontCollection.begin() ; iter != frontCollection.end() ; ++iter)
        {
            for(vector<int>::iterator jter = (*iter).S.begin() ; jter != (*iter).S.end() ; ++jter)
            {
                individuals[(*jter)].n = individuals[(*jter)].n - 1;
                if(individuals[(*jter)].n == 0)
                {
                    individuals[(*jter)].front = rank + 1;
                    Front[rank+1].push_back(individuals[(*jter)]);
                    tempCollection.push_back(individuals[(*jter)]);
                }
            }
        }
        rank++;
        frontCollection.erase(frontCollection.begin(), frontCollection.end());
        frontCollection.assign(tempCollection.begin(), tempCollection.end());
    }
}


void gacrossover(int target1, int target2, Individual *individual) //将选择的两个个体进行交叉
{
    int myMap[MAXN << 1][2];//记录一个任务的所属机器
    vector<int>:: iterator iter;

    for(int j = 0 ; j < m ; ++ j)
    {
        vector<int>().swap(individual->machine[j]);
    }

    for(int j = 0 ; j < m ; ++ j)
    {
        for(iter = Collection[target1].machine[j].begin(); iter != Collection[target1].machine[j].end(); iter ++)
        {
            myMap[(*iter)][0] = j;
        }
        for(iter = Collection[target2].machine[j].begin(); iter != Collection[target2].machine[j].end(); iter ++)
        {
            myMap[(*iter)][1] = j;
        }
    }

    for(int i = 0 ; i < n ; ++ i)
    {
        int temp = rand() % 2;
        individual -> machine[myMap[i][temp]].push_back(i);//生成一个新的个体,保存到individual中
    }
}

void light_perturbation(int segment[], int size_of_segment, int interval[]){
    int temp, k, pos1, pos2, temp1;
    int interval1[MAXN];
    interval1[0] = interval[0];
    for(int i = 1 ; i < m ; i ++){
        interval1[i] = interval1[i-1] + interval[i];
    }
    temp = rand() % m;
    while(!interval[temp]){
        temp = rand() % m;
    }
    temp1 = rand() % m;
    while(temp == temp1){
        temp1 = rand() % m;
    }

    if(interval[temp1] != 0) {
        if(temp == 0){
            pos1 = rand() % interval1[temp];
        }else{
            pos1 = interval1[temp-1] + rand() % interval[temp];
        }
        if(temp1 == 0){
            pos2 = rand() % interval1[temp1];
        }else{
            pos2 = interval1[temp1-1] + rand() % interval[temp1];
        }
        if(pos1 > pos2) {
            k = pos1;
            pos1 = pos2;
            pos2 = k;
        }
        k = segment[pos1];
        for(int i = pos1 ; i < pos2 ; i ++){
            segment[i] = segment[i + 1];
        }
        segment[pos2] = k;

        interval[temp] -= 1;
        interval[temp1] += 1;
    } else {
        interval[temp] -= 1;
        interval[temp1] += 1;
    }

}

void heavy_perturbation(int segment[], int size_of_segment, int interval[]){
    int temp, k, pos1, pos2, temp1;
    int interval1[MAXN];
    interval1[0] = interval[0];
    for(int i = 1 ; i < m ; i ++){
        interval1[i] = interval1[i-1] + interval[i];
    }

    temp = rand() % m;
    while(!interval[temp]){
        temp = rand() % m;
    }
    temp1 = rand() % m;
    while(temp == temp1){
        temp1 = rand() % m;
    }

    if(interval[temp1]) {
        if(temp == 0){
            pos1 = rand() % interval1[temp];
        }else{
            pos1 = interval1[temp-1] + rand() % interval[temp];
        }
        if(temp1 == 0){
            pos2 = rand() % interval1[temp1];
        }else{
            pos2 = interval1[temp1-1] + rand() % interval[temp1];
        }

        k = segment[pos1];
        segment[pos1] = segment[pos2];
        segment[pos2] = k;
    }else {

    }

}


void gamutation(Individual *individual)
{

    int segment[MAXN];
    int interval[MAXN];
    int size_of_segment = 0;
    vector<int>::iterator iter;

    for(int i = 0 ; i < m ; i ++)
    {
        interval[i] = individual -> machine[i].size();
        for(iter = individual -> machine[i].begin(); iter != individual -> machine[i].end(); ++ iter)
        {
            segment[size_of_segment ++] = (*iter);
        }
    }

    int temp = rand() % 2;
    if(temp == 0)
    {
        // printf("...");
        light_perturbation(segment, size_of_segment, interval);
        // printf("...");
    }
    else
    {
        //  printf("...");
        heavy_perturbation(segment, size_of_segment, interval);
        //printf("...");

    }

    for(int j = 0 ; j < m ; ++ j)
    {
        vector<int>().swap(individual->machine[j]);
    }

    temp = 0;
    for(int i = 0 ; i < m ; i ++)
    {
//        printf("interval[%d] = %d\n", i , interval[i]);
        for(int j = 0 ; j < interval[i] ; j ++)
        {
            individual->machine[i].push_back(segment[temp]);
            temp ++;
        }
    }

}

double get_max_communication(int now_rank)
{
    vector<Individual>::iterator iter;
    double max_communication = -inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_communication = (*iter).workload;
        if(max_communication < this_communication)
        {
            max_communication = this_communication;
        }
    }
    return max_communication;
}

double get_min_communication(int now_rank)
{
    vector<Individual>::iterator iter;
    double min_communication = inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_communication = (*iter).workload;
        if(min_communication > this_communication)
        {
            min_communication = this_communication;
        }
    }
    return min_communication;
}

double get_max_maxspan(int now_rank)
{
    vector<Individual>::iterator iter;
    double max_maxspan = -inf;
    double this_max_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_max_span = (*iter).workload;
        if(max_maxspan < this_max_span)
        {
            max_maxspan = this_max_span;
        }
    }
    return max_maxspan;
}

double get_min_maxspan(int now_rank)
{
    vector<Individual>::iterator iter;
    double min_maxspan = inf;
    double this_min_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_min_span = (*iter).workload;
        if(min_maxspan > this_min_span)
        {
            min_maxspan = this_min_span;
        }
    }
    return min_maxspan;
}

//计算拥挤距离
void crowdDistance(int now_rank)
{
    vector<Individual>::iterator iter;
    Individual front_individuals[MAXN];
    int length = 0;
    double max_communication = get_max_communication(now_rank);
    double min_communication = get_min_communication(now_rank);
    double max_maxspan = get_max_maxspan(now_rank);
    double min_maxspan = get_min_maxspan(now_rank);
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
    {
        front_individuals[length] = (*iter);
        length ++;
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++)
    {
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].workload - front_individuals[i-1].workload) / (max_communication - min_communication);
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp1);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++)
    {
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].makespan - front_individuals[i-1].makespan) / (max_communication - min_communication);
    }

    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp2);
//    printf("*************\n");
//    for(int i = 0 ; i < n ; i ++){
//        printf("distance = %.2lf\n", front_individuals[i].crowd_distance);
//    }
//    printf("*************\n");
    Front[now_rank].erase(Front[now_rank].begin(), Front[now_rank].end());
    for(int i = 0 ; i < length ; i ++)
    {
        Front[now_rank].push_back(front_individuals[i]);
    }
}

void insertMachine(Individual *i, int a, int b, int c) { // a = taskIndex[nowPoint], b = nowPoint, c = temp
    vector<int>:: iterator iter;
    int j = 0;
    int k = 0;
    // delete temp
    for(j = 0 ; j < m ; j ++) {
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); iter ++) {
            if((*iter) == c) {
                i->machine[j].erase(iter);


                iter = i->machine[b].begin();
                while(k < a) {
                    k ++;
                    iter ++;
                }
                i->machine[b].insert(iter, c);
                return ;
            }
        }
    }
    return ;
}

struct Node {
    int id;
    double value;
}arr[n];

int cmp4(const void *a, const void *b)
{
    return (*(Node *)a).value < (*(Node *)b).value ? -1:1;
}

int cmp5(const void *a, const void *b)
{
    return (*(Node *)a).value > (*(Node *)b).value ? -1:1;
}

int test(int task) {
    int i;
    int tot = 0;
    memset(arr, 0, sizeof(arr));
    for(i = 0 ; i < n ; i ++) {
        if(c[i][task] > 0) {
            if(doneSet.find(i) == doneSet.end() && taskUsed[i] == false) { //如果没有找到
                arr[tot].id = i;
                arr[tot ++].value = c[i][task];
            }
        }
    }
    if(!tot)return -1;
    else {
        qsort(arr, tot, sizeof(arr[0]), cmp4);
        return arr[0].id;
    }
}

int test1(int task) {
    int i;
    int tot = 0;
    memset(arr, 0, sizeof(arr));
    for(i = 0 ; i < n ; i ++) {
        if(c[i][task] > 0) {
            if(doneSet.find(i) == doneSet.end() && taskUsed[i] == false) { //如果没有找到
                arr[tot].id = i;
                arr[tot ++].value = c[i][task];
            }
        }
    }
    if(!tot)return -1;
    else {
        qsort(arr, tot, sizeof(arr[0]), cmp5);
        return arr[0].id;
    }
}

int test2(int task) {
    int i;
    for(i = 0 ; i < n ; i ++) {
        if(c[i][task] > 0) {
            if(doneSet.find(i) == doneSet.end() && taskUsed[i] == false) { //如果没有找到
                return i;
            }
        }
    }
    return -1;
}

//Repair segment
/********************************************
Function: repair_segment
Description: repair input segment of machines to fit in with requirement
Input: An individual
Output: void;
Others: Get accepted input of an individual
********************************************/
void repair_segment(Individual *i) {
    vector<int>:: iterator iter;
    /***************initialize******************/
//    printf("repair前的结果\n");
//    for(int j = 0 ; j < m ; j ++){
//        printf("第%d个机器: ", j);
//        for(iter = i->machine[j].begin(); iter != i->machine[j].end() ; iter ++){
//            printf("%d ", (*iter));
//        }
//        printf("\n");
//    }
    memset(taskIndex, 0, sizeof(taskIndex));
    int acTask = 0;
    int nowPoint = 0;
    int depentTask[m];
    int depentTask1[m];
    int depentTask2[m];
    bool dependent = false;
    memset(taskUsed, 0, sizeof(taskUsed));
    doneSet.clear();
    /*******************done********************/
    while(acTask < n) {
//        printf("actask = %d\n", acTask);
        dependent = false;
        for(nowPoint = 0 ; nowPoint < m ; nowPoint ++) { // 对所有的指针进行循环
//            printf("size = %d\n", i->machine[nowPoint].size());
//            printf("taskindex = %d\n", taskIndex[nowPoint]);
            if(i->machine[nowPoint].size() == taskIndex[nowPoint]) {
//                    dependent = true;
                    depentTask[nowPoint] = -1; // 如果指针指向最后一个任务的后面，说明这个机器已经完成，他的依赖是-1.
                    depentTask1[nowPoint] = -1;
                    continue; // 如果指针指向最后一个元素，跳出
            }
            int nowTask = i->machine[nowPoint].at(taskIndex[nowPoint]);
//            printf("nowPoint = %d\n", nowPoint);
//            printf("nowTask = %d\n", nowTask);
            depentTask[nowPoint] = test(nowTask);
            depentTask1[nowPoint] = test1(nowTask);
            depentTask2[nowPoint] = test2(nowTask);
//            printf("depentTask[nowPoint] = %d\n", depentTask[nowPoint]);
            if(depentTask[nowPoint] == -1) { // 如果能符合依赖，指针后移并且可满足的机器数+1.
                taskIndex[nowPoint] ++;
                acTask ++;
                taskUsed[nowTask] = true;
                doneSet.insert(nowTask);
                dependent = true;
                break;
            }
        }

        if(dependent == false) {
            int mode = rand() % 100;
            if(mode>=0 && mode < 45) {
                int temp = rand() % m;
                while(depentTask[temp] == -1) temp = rand() % m;
                int machine = rand() % m;
                while(temp == machine) machine = rand() % m;
    //            int number = rand() % m;
    //            while(depentTask[number] == -1) number = rand() % m;
                insertMachine(i, taskIndex[machine], machine, depentTask[temp]);
            }else if(mode >= 45 && mode <90){
                int temp = rand() % m;
                while(depentTask[temp] == -1) temp = rand() % m;
    //            int number = rand() % m;
    //            while(depentTask[number] == -1) number = rand() % m;
                insertMachine(i, taskIndex[temp], temp, depentTask1[temp]);
            } else {
                int machine = rand() % m;
                int temp = rand() % m;
                while(depentTask[temp] == -1) temp = rand() % m;
                insertMachine(i, taskIndex[machine], machine, depentTask2[temp]);
            }
        }
    }
//    evaluate_objective(i);
//
//        printf("[");
//        printf("%.2lf,", i->makespan);
//        printf("%.2lf", i->workload);
//        printf("],");
//        printf("\n");
}

void swap_machine(Individual *individual, int nowM, int nowPos, int toM, int toPos) {
    vector<int>::iterator iter;
    int temp = individual->machine[nowM][nowPos];
    individual->machine[nowM][nowPos] = individual->machine[toM][toPos];
    individual->machine[toM][toPos] = temp;
//    for(int j = 0 ; j < m ; j ++){
//        printf("第%d个机器: ", j);
//        for(iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//            printf("%d ", (*iter));
//        }
//        printf("\n");
//    }
}

bool check_machine(Individual *individual) {
    vector<int>:: iterator iter;
    /***************initialize******************/
    memset(taskIndex, 0, sizeof(taskIndex));
    int acTask = 0;
    int nowPoint = 0;
    int depentTask[m];
    bool dependent = false;
    memset(taskUsed, 0, sizeof(taskUsed));
    doneSet.clear();
    /*******************done********************/
    while(acTask < n) {
        dependent = false;
        for(nowPoint = 0 ; nowPoint < m ; nowPoint ++) { // 对所有的指针进行循环
            if(individual->machine[nowPoint].size() == taskIndex[nowPoint]) {
                    depentTask[nowPoint] = -1; // 如果指针指向最后一个任务的后面，说明这个机器已经完成，他的依赖是-1.
                    continue; // 如果指针指向最后一个元素，跳出
            }
            int nowTask = individual->machine[nowPoint].at(taskIndex[nowPoint]);
            depentTask[nowPoint] = test(nowTask);
            if(depentTask[nowPoint] == -1) { // 如果能符合依赖，指针后移并且可满足的机器数+1.
                taskIndex[nowPoint] ++;
                acTask ++;
                if(acTask == n) return true;
                taskUsed[nowTask] = true;
                doneSet.insert(nowTask);
                dependent = true;
                break;
            }
        }

        if(dependent == false) {
            return false;
        }
    }
}

void swap_localsearch(Individual *individual) {
    /***************initialize******************/
    int flag[n];
    int k = 1;
    int kMax = 2*n;
    int nowMachine;
    int nowPos;
    int toMachine;
    int toPos;
    int segmentSize[m];
    Individual temp_individual = *individual;
    Individual best_individual = *individual;
    for(int l = 0 ; l < m ; l ++) {
        segmentSize[l] = temp_individual.machine[l].size();
    }
    /******************done*********************/
    while(k <= kMax) {
        evaluate_objective(&best_individual);
        double makespan = best_individual.makespan;
        double workload = best_individual.workload;
        //record non dominated solution

        // random generate a task
        nowMachine = rand() % m;
        if(segmentSize[nowMachine] == 0) continue;
        nowPos = rand() % segmentSize[nowMachine];
        // task done
        // find best neighbor
        for(toMachine = 0 ; toMachine < m ; toMachine ++) {
            if(segmentSize[toMachine]) for(toPos = 0 ; toPos < segmentSize[toMachine] ; toPos ++) {
                if(nowMachine != toMachine || nowPos != toPos) {
                    swap_machine(&temp_individual, nowMachine, nowPos, toMachine, toPos);
                    if(check_machine(&temp_individual)) { //如果符合条件
                        evaluate_objective(&temp_individual);

                        if(temp_individual.makespan < makespan && temp_individual.workload < workload) {
                            makespan = temp_individual.makespan;
                            workload = temp_individual.workload;
                            best_individual = temp_individual;
                        }
                    }
                    temp_individual = *individual;
                }
            }
        }
        k ++;
        if(best_individual.makespan < individual->makespan && best_individual.workload < individual->workload) {
            individual = &best_individual;
            k = 1;
        }
    }

}

void make_new_pop(Individual individuals[], int length)
{

    vector<int>::iterator iter;
    int flag_individual[MAXN]; //标记这个个体是否被选择过
    //tournament_selection
    memset(flag_individual, 0, sizeof(flag_individual)); //初始全部未被选择
    for(int i = 0 ; i < length ; i ++)
    {
        int target1 = rand() % ((length<<1));
        while(flag_individual[target1] != 0)
        {
            target1 = rand() % ((length<<1));
        }
        //   printf("1新生成机器%d\n",i);
        flag_individual[target1] = 1;
        int target2 = rand() % ((length<<1));
        while(flag_individual[target2] != 0)
        {
            target2 = rand() % ((length<<1));
        } //随机选择两个目标
        //     printf("2新生成机器%d\n",i);
        flag_individual[target2] = 1;
//        printf("target1 = %d, target2 = %d\n", target1, target2);
        int temp = rand() % 10;
        gacrossover(target1, target2, &new_individual);
        //    printf("3新生成机器%d\n",i);
        if(temp >= 0 && temp < 9) {}
        else
        {
            //        printf("4新生成机器%d\n",i);
            gamutation(&new_individual);
            //        printf("5新生成机器%d\n",i);
        }
//        printf("temp = %d\n", temp);
//        printf("新生成机器%d\n",i);
//        for(int j = 0 ; j < m ; j ++){
//            printf("第%d个机器: ", j);
//            for(iter = new_individual.machine[j].begin(); iter != new_individual.machine[j].end() ; iter ++){
//                printf("%d ", (*iter));
//            }
//            printf("\n");
//        }
          //Swap_localsearch(&new_individual);

        repair_segment(&new_individual);
        swap_localsearch(&new_individual);
        copy_individual(&individuals[length + i], &new_individual);

//        gacrossover(target1, target2, &new_individual);
//        gamutation(&new_individual);


    }
}

double get_min_time_in_array(double arr[])
{
    double min_time = inf;
    int min_id;
    for(int i = 0 ; i < m ; i ++)
    {
        if(min_time > arr[i])
        {
            min_time = arr[i];
            min_id = i;
        }
    }
    return min_time;
}

double get_min_id_in_array(double arr[])
{
    double min_time = inf;
    int min_id;
    for(int i = 0 ; i < m ; i ++)
    {
        if(min_time > arr[i])
        {
            min_time = arr[i];
            min_id = i;
        }
    }
    return min_id;
}

void greedy_for_workload()
{
    double now_span[MAXN];
    memset(now_span, 0, sizeof(now_span));
    for(int i = 0 ; i < n ; i ++)
    {
        time_in_machine[i].id = i;
        time_in_machine[i].time = t[0][i];
    }
    qsort(time_in_machine, n, sizeof(time_in_machine[0]), cmp3);
    for(int i = 0 ; i < n ; i ++)
    {
        if(i >= 0 && i < m)
        {
            Collection[0].machine[i].push_back(time_in_machine[i].id);
            now_span[i] += time_in_machine[i].time;
        }
        else
        {
            int pos = get_min_id_in_array(now_span);
            Collection[0].machine[pos].push_back(time_in_machine[i].id);
            now_span[pos] += time_in_machine[i].time;
        }
    }
}

void greedy_for_communication()
{
    vector<int>::iterator iter;
    bool flag;
    int machine_number[MAXN];
    memset(machine_number, 0, sizeof(machine_number));
    int max_number = n / m;
//    for(int i = 0 ; i < n ; i ++){
//        flag = false;
//        for(int j = 0 ; j < m ; j ++){
//            for(iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end(); ++ iter){
//
//                if(c[i][(*iter)] > 0 && machine_number[j] <= max_number){
//                    Collection[1].machine[j].push_back(i);
//                    machine_number[j] ++;
//                    flag = true;
//                    break;
//                }
//            }
//            if(flag) break;
//        }
//        if(!flag){
//            int pos;
//            int minn = inf;
//            for(int k = 0 ; k < m ; k ++){
//                if(machine_number[k] < minn && machine_number[k] <= max_number){
//                    minn = machine_number[k];
//                    pos = k;
//                }
//            }
//            Collection[1].machine[pos].push_back(i);
//            machine_number[pos] ++;
//        }
//    }
//    mp3_decoder
    Collection[1].machine[2].push_back(0);
    Collection[1].machine[2].push_back(1);
    Collection[1].machine[2].push_back(6);
    Collection[1].machine[2].push_back(4);
    Collection[1].machine[2].push_back(3);
    Collection[1].machine[2].push_back(5);
    Collection[1].machine[1].push_back(2);
    Collection[1].machine[1].push_back(7);
    Collection[1].machine[0].push_back(8);
    Collection[1].machine[0].push_back(9);
    //h264_dep
//        Collection[1].machine[0].push_back(0);
//        Collection[1].machine[0].push_back(4);
//        Collection[1].machine[0].push_back(6);
//        Collection[1].machine[0].push_back(7);
//        Collection[1].machine[0].push_back(8);
//        Collection[1].machine[0].push_back(11);
//        Collection[1].machine[0].push_back(10);
//        Collection[1].machine[1].push_back(3);
//        Collection[1].machine[1].push_back(2);
//        Collection[1].machine[1].push_back(9);
//        Collection[1].machine[1].push_back(13);
//        Collection[1].machine[1].push_back(12);
//        Collection[1].machine[1].push_back(5);
//        Collection[1].machine[1].push_back(1);
    repair_segment(&Collection[1]);

        for(int j = 0 ; j < m ; j ++){
            printf("第%d个机器: ", j);
            for(iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end() ; iter ++){
                printf("%d ", (*iter));
            }
            printf("\n");
        }
    evaluate_objective(&Collection[1]);
//        Collection[1].machine[5].push_back(11);
//        Collection[1].machine[6].push_back(7);
//        Collection[1].machine[7].push_back(9);
//        Collection[1].machine[7].push_back(12);

//    double span = 0;
//    for(int j = 0 ; j < number_of_machines ; j ++){
//        double tspan = 0;
//        for(vector<int>::iterator iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end(); ++ iter){
//            tspan += t[j][(*iter)];
//            for(int jj = 0 ; jj < number_of_machines ; jj ++){
//                if(jj != j){
//                    for(vector<int>::iterator jter = Collection[1].machine[jj].begin(); jter != Collection[1].machine[jj].end(); ++ jter){
//                        Collection[1].communication_cost += c[(*iter)][(*jter)];
//                    }
//                }
//            }
//        }
//        printf("tspan = %.2lf\n", tspan);
//        span += abs(tspan - avg);
//    }
//    evaluate_objective(&Collection[1]);
//    printf("span = %.2lf\n", Collection[1].maxspan);
//    printf("comm = %.2lf\n", Collection[1].communication_cost);
//    for(int j = 0 ; j < m ; j ++){
//        printf("第%d台机器的序列", j);
//        for(vector<int>::iterator iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end(); iter ++){
//            printf("%d ", (*iter));
//        }
//        printf("\n");
//    }
//    printf("\n");
//    getchar();getchar();
}

void greedy_with_topo() {
    int in[n];
    int number_in_machine[m];
    int ac_task = 0;
    int now_machine = 0;
    int maxN = n / m;
    int vis[n];
    for(int i = 0 ; i < n ; i ++) {
        in[i] = isdep[0][i];
    }
    memset(number_in_machine, 0, sizeof(number_in_machine));
    memset(vis, 0, sizeof(vis));

    while(ac_task < n) {
        for(int i = 0 ; i < n ; i ++) {
            if(!in[i] && !vis[i]) {
                ac_task ++;
                Collection[0].machine[now_machine].push_back(i);
                vis[i] = 1;
                for(int j = 0 ; j < n ;j ++) {
                    if(c[i][j] > 0) {
                        in[j] --;
                    }
                }
                if(Collection[0].machine[now_machine].size() >= maxN && now_machine < m - 1) {
                    now_machine ++;
                }

            }
        }
    }

}

void init()
{

    set<int> flag_machine;
    stack<int> segment;
    set<int> interval;
    int kk, kkk;
    vector<int>::iterator iter;
    set<int>::iterator iiter;
    int tempkk;
    for(int i = 0 ; i < pop*2 ; i ++)
    {
        for(int j = 0 ; j < m ; j ++)
        {
            Collection[i].machine[j].erase(Collection[i].machine[j].begin(), Collection[i].machine[j].end());
        }
    }

    //greedy_for_workload();
    //greedy_for_communication();
    greedy_with_topo();
//    printf("greedy.... done\n");

    for(int i = 1 ; i < pop*2 ; i ++)
    {
        flag_machine.clear();
        while(!segment.empty())
        {
            segment.pop();
        }

        for(int j = 0 ; j < n * 2 ; j ++)
        {
            int temp = rand() % n;
            if(flag_machine.insert(temp).second)
            {
                segment.push(temp);
            }
        }
        for(int k = 0 ; k < n ; k ++)
        {
            if(flag_machine.insert(k).second)
            {
                segment.push(k);
            }
        }

        interval.clear();
        kk = m - 1;
        while(kk)
        {
            int temp = rand() % (n);
            if(temp == 0) temp = 1;
            while(!interval.insert(temp).second)
            {
                temp = rand() % (n);
                if(temp == 0) temp = 1;
            }
            kk --;
        }
//        printf("生成的序列是:\n");
//        while(!segment.empty()){
//            printf("%d ", segment.top());
//            segment.pop();
//        }
//        printf("*********************\n");
        kkk = 0;
        for(iiter = interval.begin(); iiter != interval.end(); iiter ++)
        {
//            printf("interval中的值=%d\n", (*iiter));
            if(iiter == interval.begin())
            {
                kk = (*iiter);
            }
            else
            {
//                printf("intervar=%d kk=%d\n", (*iiter), kk);
                kk = (*iiter) - kk;
            }
//            kk = (*iiter);
//            printf("区间大小1=%d\n", kk);
            tempkk = kk;
            while(tempkk --)
            {
                Collection[i].machine[kkk].push_back(segment.top());
                segment.pop();
            }
            kkk ++;
            kk = (*iiter);
        }

        kk = n - kk;
        tempkk = kk;
        while(tempkk --)
        {
            Collection[i].machine[kkk].push_back(segment.top());
            segment.pop();
        }

//        printf("repair... begin\n");
        repair_segment(&Collection[i]);
//        printf("repair... done\n");

//        for(int j = 0 ; j < m ; j ++){
//            printf("第%d台机器的序列", j);
//            for(iter = Collection[i].machine[j].begin(); iter != Collection[i].machine[j].end(); iter ++){
//                printf("%d ", (*iter));
//            }
//            printf("\n");
//        }
//        printf("\n");
//        getchar();
    }
}

//Main Process
void solve()
{

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

    int t = 0;
    printf("init...begin\n");
    init();
    printf("init...done\n");

    while(t < gen)
    {
        printf("*****************************t=%d*****************************\n", t);

        int P_size = 0;
        int now_rank = 1;

        make_new_pop(Collection, pop);
        //printf("t2=%d\n", t);
        for(int i = 0 ; i < pop*2 ; i ++)
        {
            evaluate_objective(&Collection[i]);
        }
        //printf("evaluate done ...\n");
        non_domination_sort(Collection, pop * 2, false);
//         for(int i = 0 ; i < pop*2 ; i ++){
//            printf("i=%d\n", i);
//            printf("makespan = %.2lf\n", Collection[i].makespan);
//            printf("workload = %.2lf\n", Collection[i].workload);
//            printf("rank = %d\n", Collection[i].front);
//        }

        while(1)
        {
            if(P_size + Front[now_rank].size() > pop)
            {
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
            {
                Collection[P_size] = (*iter);
                P_size ++;
            }
            now_rank ++;
        }
        crowdDistance(now_rank);
        while(1)
        {
            if(P_size > pop)
            {
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
            {
                Collection[P_size] = (*iter);
                P_size ++;
            }
        }

//            for(int i = 0 ; i < pop*2 ; i ++){
//                evaluate_objective(&Collection[i]);
//                printf("i=%d\n", i);
//                printf("maxspan = %.2lf\n", Collection[i].maxspan);
//                printf("communicate = %.2lf\n", Collection[i].communication_cost);
//                printf("rank = %d\n", Collection[i].front);
//            }
        t ++;
    }
//    for(int i = 0 ; i < pop ; i ++){
//
//        printf("[");
//        printf("%.2lf,", Collection[i].maxspan);
//        printf("%.2lf,rank=%d", Collection[i].communication_cost, Collection[i].front);
//        printf("],", Collection[i].front);
//    }

    non_domination_sort(Collection, pop * 2, true);

    int tot = 0;
    for(int i = 0 ; i < pop * 2 ; i ++)
    {
        if(Collection[i].front == 1)
        {
            printf("[");
            printf("%.2lf,", Collection[i].makespan);
            printf("%.2lf", Collection[i].workload);
            printf("],");
            printf("\n");
            tot ++;
        }
    }
    printf("tot = %d\n", tot);
}

//void get_segment_array(Individual *i, int **)
int main()
{
    srand(1);
    freopen("TMNR.dat", "r", stdin);
    freopen("MOHA-machine4", "w", stdout);
    solve();
    return 0;
}
