#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<vector>
#include<math.h>
#include<set>
#include<stack>

using namespace std;

const int MAXN = 2000;
const int number_of_tasks = 14;
const int number_of_machines = 8;
const int m = number_of_machines;
const int n = number_of_tasks;
const int inf = 0x3f3f3f3f;

//*******
//**pop - Population size
//**gen - Total number of generations
int pop, gen;


//*******
//**number_of_objectives - the number of objective function
//**number_of_decision_variables - number of decision variables
//**min_range_of_decision_variable[] - minimum possible value for each decision variable
//**max_range_of_decision_variable[] - maximum possible value for each decision variable

int number_of_objectives, number_of_decision_variables, min_range_of_decision_variable[MAXN], max_range_of_decision_variable[MAXN];
//int n, m;


//*******
//**目标函数   Minimize communication cost
//**目标函数   Minimize the balance of processor workload


//*******
//**input
//**t(m,n) Execution time of a task on a processor
//**c(m,n) Communication cost between two tasks

double t[m][n] = {
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14}
};
double c[n][n] = {
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14}
};
bool uesd[MAXN];

struct Individual{
    vector<int> machine[number_of_machines]; //save permutation of the tasks
    double communication_cost; //object 1
    double maxspan; //object 2
    int front; //rank of domination
    vector< Individual > S; //the collections dominated by this Individual
    int n;// count of dominated solution
    double dfitness; //fitness
    double crowd_distance;
}Collection[100], new_individual;

int cmp(const void *a, const void *b){
    return (*(Individual *)a).communication_cost > (*(Individual *)b).communication_cost ? 1:-1;
}

void evaluate_objective(Individual *i){
    vector<int>:: iterator iter;
    vector<int>:: iterator jter;
    i->n = inf;
    i->maxspan = 0;
    i->communication_cost = 0;
    for(int j = 0 ; j < number_of_machines ; j ++){
        double span = 0;
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); ++ iter){
            span += t[j][(*iter)];
            for(jter = i->machine[j].begin(); jter != i->machine[j].end(); ++ jter){
                if(c[j][(*iter)] > 0 && iter != jter){
                    i->communication_cost += c[j][(*iter)];
                }
            }
        }
        if(span > i->maxspan){
            i->maxspan = span;
        }
    }
    i->dfitness = i->maxspan + i->communication_cost;
}

// 非支配排序
void non_domination_sort(Individual individuals[], int length){
    vector< Individual > frontCollection;
    vector< Individual > tempCollection;
    int rank = 1;
    for(int i = 0 ; i < length ; i ++){
        //initial
        individuals[i].S.erase(individuals[i].S.begin(), individuals[i].S.end());
        individuals[i].n = 0;
        for(int j = 0 ; j < length ; j ++){
            // if individual[i] dominate individual[j]
            if(individuals[i].maxspan < individuals[j].maxspan && individuals[i].communication_cost < individuals[j].communication_cost){
                // let individual[j] added to the S of the individual[i]
                individuals[i].S.push_back(individuals[j]);
            }else if(individuals[j].maxspan < individuals[i].maxspan && individuals[j].communication_cost < individuals[i].communication_cost){
                individuals[i].n = individuals[i].n + 1;
            }
        }
    }
    for(int i = 0 ; i < length ; i ++){
        if(individuals[i].n == 0){
            individuals[i].front = rank;
            frontCollection.push_back(individuals[i]);
        }
    }
    while(!frontCollection.empty()){
        tempCollection.erase(tempCollection.begin(), tempCollection.end());
        for(vector<Individual>::iterator iter = frontCollection.begin() ; iter != frontCollection.end() ; ++iter){
            for(vector<Individual>::iterator jter = (*iter).S.begin() ; jter != (*iter).S.end() ; ++jter){
                (*jter).n = (*jter).n - 1;
                if((*jter).n == 0){
                    (*iter).front = rank + 1;
                    tempCollection.push_back((*iter));
                }
            }
        }
        rank++;
        frontCollection.erase(frontCollection.begin(), frontCollection.end());
        frontCollection.assign(tempCollection.begin(), tempCollection.end());
    }
}



//round robin selection
//int round_robin_selection(Individual individuals[], int length){
//    int target = 0;
//    double total_fitness = 0;
//    for(int i = 0 ; i < length ; i ++){
//        total_fitness = total_fitness + individuals[i].dfitness;
//    }
//    double dRange = (((rand()+ rand())%100001)/(100000 + 0.0000001)) * total_fitness;
//    double dCursor = 0;
//    for(int i = 0 ; i < length ; i ++){
//        dCursor = dCursor + individuals[i].dfitness;
//        target ++;
//        if(dCursor > dRange){
//            break;
//        }
//    }
//    return target;
//}

//void gacrossover(Individual individuals[], int length, double proc){
//
//    //crossover
//    //random point of crossover
//    //proc is the possiblity of crossover
//
//    int target1 = round_robin_selection(individuals, length);
//    int target2 = round_robin_selection(individuals, length);
//    while(target1 == target2){
//        target2 = round_robin_selection(individuals, length);
//    }
//
//    //crossover
//    double temp = rand();
//    if(temp < proc) return ;
//    int m1 = rand() * (m-1);
//    int m2 = rand() * (m-1);
////    vector<int>::size_type x1 = m1;
////    vector<int>::size_type x2 = m2;
//    //change machine
//    individuals[target1].machine[m1].swap(individuals[target2].machine[m2]);
//
//}

void gacrossover(int target1, int target2, Individual *individual){//将选择的两个个体进行交叉
    int myMap[MAXN << 1][0];//记录一个任务的所属机器
    vector<int>:: iterator iter;
    for(int j = 0 ; j < m ; ++ j){
        for(iter = Collection[target1].machine[j].begin(); iter != Collection[target1].machine[j].end(); iter ++){
            myMap[(*iter)][0] = j;
        }
        for(iter = Collection[target2].machine[j].begin(); iter != Collection[target2].machine[j].end(); iter ++){
            myMap[(*iter)][1] = j;
        }
    }
    for(int i = 0 ; i < n ; ++ i){
        int temp = round(rand() / 2);
        individual -> machine[myMap[i][temp]].push_back(i);//生成一个新的个体
    }
}

void light_perturbation(int segment[], int size_of_segment){

}

void heavy_perturbation(int segment[], int size_of_segment){

}


void gamutation(Individual *individual){

    int segment[MAXN];
    stack<int> interval;
    int size_of_segment = 0;
    vector<int>::iterator iter;

    for(int i = 0 ; i < m ; i ++){
        interval.push(individual -> machine[i].size());
        for(iter = individual -> machine[i].begin(); iter != individual -> machine[i].end(); ++ iter){
            segment[size_of_segment ++] = (*iter);
        }
    }


}

//计算拥挤距离
void crowdDistance(Individual individuals[], int length){
    qsort(individuals, length, sizeof(individuals[0]), cmp);
    individuals[0].crowd_distance = inf;
    individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++){
        individuals[i].crowd_distance += individuals[i+1].communication_cost - individuals[i-1].communication_cost;
    }
}

void make_new_pop(Individual individuals[], int length){

    int flag_individual[MAXN]; //标记这个个体是否被选择过
    //tournament_selection
    memset(flag_individual, 0, sizeof(flag_individual)); //初始全部未被选择
    int target1 = rand() % (length - 1);
    while(flag_individual[target1] != 0){
        target1 = rand() % (length - 1);
    }
    int target2 = rand() % (length - 1);
    while(flag_individual[target2] != 0){
        target2 = rand() % (length - 1);
    } //随机选择两个目标
    flag_individual[target1] = 1;
    flag_individual[target2] = 1;
    gacrossover(target1, target2, &new_individual);

}

void init(){
    set<int> flag_machine;
    stack<int> segment;
    set<int> interval;
    int kk, kkk;
    vector<int>::iterator iter;
    set<int>::iterator iiter;
    int tempkk;
    for(int i = 0 ; i < pop ; i ++){
        flag_machine.clear();
        while(!segment.empty()){
            segment.pop();
        }
        for(int j = 0 ; j < n * 2 ; j ++){
            int temp = int(rand() % n);
            if(flag_machine.insert(temp).second){
                segment.push(temp);
            }
        }
        for(int k = 0 ; k < n ; k ++){
            if(flag_machine.insert(k).second){
                segment.push(k);
            }
        }
        interval.clear();
        kk = m - 1;
        while(kk){
            int temp = int(rand() % (n));
            if(temp == 0) temp = 1;
            while(!interval.insert(temp).second){
                temp = int(rand() % (n));
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
        for(iiter = interval.begin(); iiter != interval.end(); iiter ++){
//            printf("interval中的值=%d\n", (*iiter));
            if(iiter == interval.begin()){
                kk = (*iiter);
            }else{
//                printf("intervar=%d kk=%d\n", (*iiter), kk);
                kk = (*iiter) - kk;
            }
//            kk = (*iiter);
//            printf("区间大小1=%d\n", kk);
            tempkk = kk;
            while(tempkk --){
                Collection[i].machine[kkk].push_back(segment.top());
                segment.pop();
            }
            kkk ++;
            kk = (*iiter);
        }

        kk = n - kk;
        tempkk = kk;
        while(tempkk --){
            Collection[i].machine[kkk].push_back(segment.top());
            segment.pop();
        }



//        for(int j = 0 ; j < m ; j ++){
//            printf("第%d台机器的序列", j);
//            for(iter = Collection[i].machine[j].begin(); iter != Collection[i].machine[j].end(); iter ++){
//                printf("%d ", (*iter));
//            }
//            printf("\n");
//        }
//        getchar();
    }
}

//Main Process
void solve(){
    while(~scanf("%d", &pop)){
        init();

        make_new_pop(Collection, pop);


        for(int i = 0 ; i < pop ; i ++){
            evaluate_objective(&Collection[i]);
            printf("maxspan = %.2lf\n", Collection[i].maxspan);
            printf("communicate = %.2lf\n", Collection[i].communication_cost);
        }
    }


}

int main(){



    solve();
//    printf("NSGA-II: Please enter the population size and number of generations as input arguments.\npopulation size: ");
//    scanf("%d", &pop);
//    printf("number of generations: ");
//    scanf("%d", &gen);
//
//    printf("Input the number of objective: ");
//    scanf("%d", &number_of_objectives);
//    printf("Input the number of decision variables: ");
//    scanf("%d", &number_of_decision_variables);
//
//    for(int i = 1; i <= number_of_decision_variables; i ++){
//        printf("Input the minimum value for decision variable ");
//        scanf("%d", min_range_of_decision_variable[i]);
//        printf("Input the maximum value for decision variable ");
//        scanf("%d", max_range_of_decision_variable[i]);
//    }

    //Initialize the population


    //Sort the initialized population


    //**test area
    //**
    //**
//    int a[100] = {1};
//    int b[100] = {1,2};
//    int c[100] = {1,2,3};
//    int d[100] = {1,2,3,4};
//    vector<int> t(a,a+1);
//    Individual test;
//    test.machine.insert(test.machine.end(), t);
//    t.erase(t.begin(), t.end());
//    t.push_back(b[0]);t.push_back(b[1]);
//    test.machine.insert(test.machine.end(), t);
//    t.erase(t.begin(), t.end());
//    t.push_back(c[0]);t.push_back(c[1]);t.push_back(c[2]);
//    test.machine.insert(test.machine.end(), t);
//    t.erase(t.begin(), t.end());
//    t.push_back(d[0]);t.push_back(d[1]);t.push_back(d[2]);t.push_back(d[3]);
//    test.machine.insert(test.machine.end(), t);
//    evaluate_objective(&test);
    //***********************************************************
    return 0;
}



