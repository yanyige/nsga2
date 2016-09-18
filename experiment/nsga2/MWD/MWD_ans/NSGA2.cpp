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
const int number_of_tasks = 12;
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
    light_perturbation(segment, size_of_segment, interval);
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

int test(int task) {
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
                    continue; // 如果指针指向最后一个元素，跳出
            }
            int nowTask = i->machine[nowPoint].at(taskIndex[nowPoint]);
//            printf("nowPoint = %d\n", nowPoint);
//            printf("nowTask = %d\n", nowTask);
            depentTask[nowPoint] = test(nowTask);
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
//            printf("不符合依赖\n");
            int machine = rand() % m;

            int temp = rand() % m;
//            printf("选择的机器是%d\n", temp);
            while(depentTask[temp] == -1) temp = rand() % m;
//            int number = rand() % m;
//            while(depentTask[number] == -1) number = rand() % m;
            insertMachine(i, taskIndex[machine], machine, depentTask[temp]);
//            printf("插入后的结果\n");
//            for(int j = 0 ; j < m ; j ++){
//                printf("第%d个机器: ", j);
//                for(iter = i->machine[j].begin(); iter != i->machine[j].end() ; iter ++){
//                    printf("%d ", (*iter));
//                }
//                printf("\n");
//            }
        }
    }
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
            gamutation(&new_individual);
        }

        repair_segment(&new_individual);
        copy_individual(&individuals[length + i], &new_individual);
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

    for(int i = 0 ; i < pop*2 ; i ++)
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
        kkk = 0;
        for(iiter = interval.begin(); iiter != interval.end(); iiter ++)
        {
            if(iiter == interval.begin())
            {
                kk = (*iiter);
            }
            else
            {
                kk = (*iiter) - kk;
            }
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

        repair_segment(&Collection[i]);
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
    init();
    while(t < gen)
    {
        printf("*****************************t=%d*****************************\n", t);

        int P_size = 0;
        int now_rank = 1;

        make_new_pop(Collection, pop);
        for(int i = 0 ; i < pop*2 ; i ++)
        {
            evaluate_objective(&Collection[i]);
        }
        non_domination_sort(Collection, pop * 2, false);
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
        t ++;
    }

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
    freopen("MWD.dat", "r", stdin);
    freopen("NSGA2-machine4.txt", "w", stdout);
    solve();
    return 0;
}
