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

//int n, m;


//*******
//**目标函数   Minimize communication cost
//**目标函数   Minimize the balance of processor workload


//*******
//**input
//**t(m,n) Execution time of a task on a processor
//**c(m,n) Communication cost between two tasks

double t[m][n];
struct time_table{
    double time;
    int id;
}time_in_machine[n];

double c[n][n];
bool uesd[MAXN];

struct Individual{
    vector<int> machine[number_of_machines]; //save permutation of the tasks
    double communication_cost; //object 1
    double maxspan; //object 2
    int front; //rank of domination
    vector< int > S; //the collections dominated by this Individual
    int n;// count of dominated solution
    double dfitness; //fitness
    double crowd_distance;
}Collection[MAXN], new_individual;
vector<Individual> Front[MAXN];

int cmp(const void *a, const void *b){
    return (*(Individual *)a).communication_cost > (*(Individual *)b).communication_cost ? 1:-1;
}

int cmp1(const void *a, const void *b){
    return (*(Individual *)a).maxspan > (*(Individual *)b).maxspan ? 1:-1;
}

int cmp2(const void *a, const void *b){
    return (*(Individual *)a).crowd_distance > (*(Individual *)b).crowd_distance ? -1:1;
}

int cmp3(const void *a, const void *b){
    return (*(time_table *)a).time > (*(time_table *)b).time ? -1 : 1;
}

void copy_individual(Individual *i, Individual *j){
    vector<int>:: iterator iter;
    for(int ii = 0 ; ii < m ; ii ++){
        i->machine[ii].erase(i->machine[ii].begin(), i->machine[ii].end());
    }
    for(int ii = 0 ; ii < m ; ii ++){
        for(iter = j->machine[ii].begin(); iter != j->machine[ii].end(); ++ iter){
            i->machine[ii].push_back((*iter));
        }
    }
}

double abs(double t){
    return t>0?t:-t;
}

void evaluate_objective(Individual *i){
    vector<int>:: iterator iter;
    vector<int>:: iterator jter;
    i->n = inf;
    i->maxspan = 0;
    i->communication_cost = 0;
    double avg = 0;
    for(int j = 0 ; j < n ; j ++){
        avg += t[0][j];
    }
    avg /= m;
    double span = 0;
    for(int j = 0 ; j < number_of_machines ; j ++){
        double tspan = 0;
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); ++ iter){
            tspan += t[j][(*iter)];
            for(int jj = 0 ; jj < number_of_machines ; jj ++){
                if(jj != j){
                    for(jter = i->machine[jj].begin(); jter != i->machine[jj].end(); ++ jter){
                        i->communication_cost += c[(*iter)][(*jter)];
                    }
                }
            }
        }
        span += abs(tspan - avg);
    }
    i->maxspan = span;

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
void non_domination_sort(Individual individuals[], int length){
    vector< Individual > frontCollection;
    vector< Individual > tempCollection;
    int rank = 1;
    for(int i = 0 ; i < length ; i ++){
        //initial
        individuals[i].S.erase(individuals[i].S.begin(), individuals[i].S.end());
        individuals[i].n = 0;
        for(int j = 0 ; j < length ; j ++){
            if(i!=j){
                // if individual[i] dominate individual[j]
                if(individuals[i].maxspan < individuals[j].maxspan && individuals[i].communication_cost < individuals[j].communication_cost){
                    // let individual[j] added to the S of the individual[i]
                    individuals[i].S.push_back(j);
                }else if(individuals[j].maxspan < individuals[i].maxspan && individuals[j].communication_cost < individuals[i].communication_cost){
                    individuals[i].n = individuals[i].n + 1;
                }
            }
        }
    }
    for(int i = 0 ; i < MAXN ; i ++){
        vector<Individual>().swap(Front[i]);
    }
    for(int i = 0 ; i < length ; i ++){
        if(individuals[i].n == 0){
            individuals[i].front = rank;
            frontCollection.push_back(individuals[i]);
            Front[rank].push_back(individuals[i]);
        }
    }

    while(!frontCollection.empty()){
        tempCollection.erase(tempCollection.begin(), tempCollection.end());
        for(vector<Individual>::iterator iter = frontCollection.begin() ; iter != frontCollection.end() ; ++iter){
            for(vector<int>::iterator jter = (*iter).S.begin() ; jter != (*iter).S.end() ; ++jter){
                individuals[(*jter)].n = individuals[(*jter)].n - 1;
                if(individuals[(*jter)].n == 0){
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


void gacrossover(int target1, int target2, Individual *individual){//将选择的两个个体进行交叉
    int myMap[MAXN << 1][2];//记录一个任务的所属机器
    vector<int>:: iterator iter;

    for(int j = 0 ; j < m ; ++ j){
        vector<int>().swap(individual->machine[j]);
    }

    for(int j = 0 ; j < m ; ++ j){
        for(iter = Collection[target1].machine[j].begin(); iter != Collection[target1].machine[j].end(); iter ++){
            myMap[(*iter)][0] = j;
        }
        for(iter = Collection[target2].machine[j].begin(); iter != Collection[target2].machine[j].end(); iter ++){
            myMap[(*iter)][1] = j;
        }
    }

    for(int i = 0 ; i < n ; ++ i){
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
    while(temp == temp1 || !interval[temp1]){
        temp1 = rand() % m;
    }

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
    for(int i = pos1 ; i < pos2 ; i ++){
        segment[i] = segment[i + 1];
    }
    segment[pos2] = k;

    interval[temp] -= 1;
    interval[temp1] += 1;
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
    while(temp == temp1 || !interval[temp1]){
        temp1 = rand() % m;
    }

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

}


void gamutation(Individual *individual){

    int segment[MAXN];
    int interval[MAXN];
    int size_of_segment = 0;
    vector<int>::iterator iter;

    for(int i = 0 ; i < m ; i ++){
        interval[i] = individual -> machine[i].size();
        for(iter = individual -> machine[i].begin(); iter != individual -> machine[i].end(); ++ iter){
            segment[size_of_segment ++] = (*iter);
        }
    }

    int temp = rand() % 2;
    if(temp == 0) {
        light_perturbation(segment, size_of_segment, interval);
    }
    else {
        heavy_perturbation(segment, size_of_segment, interval);

    }

    for(int j = 0 ; j < m ; ++ j){
        vector<int>().swap(individual->machine[j]);
    }

    temp = 0;
    for(int i = 0 ; i < m ; i ++){
//        printf("interval[%d] = %d\n", i , interval[i]);
        for(int j = 0 ; j < interval[i] ; j ++){
            individual->machine[i].push_back(segment[temp]);
            temp ++;
        }
    }

}

double get_max_communication(int now_rank){
    vector<Individual>::iterator iter;
    double max_communication = -inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter){
        this_communication = (*iter).communication_cost;
        if(max_communication < this_communication){
            max_communication = this_communication;
        }
    }
    return max_communication;
}

double get_min_communication(int now_rank){
    vector<Individual>::iterator iter;
    double min_communication = inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter){
        this_communication = (*iter).communication_cost;
        if(min_communication > this_communication){
            min_communication = this_communication;
        }
    }
    return min_communication;
}

double get_max_maxspan(int now_rank){
    vector<Individual>::iterator iter;
    double max_maxspan = -inf;
    double this_max_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter){
        this_max_span = (*iter).communication_cost;
        if(max_maxspan < this_max_span){
            max_maxspan = this_max_span;
        }
    }
    return max_maxspan;
}

double get_min_maxspan(int now_rank){
    vector<Individual>::iterator iter;
    double min_maxspan = inf;
    double this_min_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter){
        this_min_span = (*iter).communication_cost;
        if(min_maxspan > this_min_span){
            min_maxspan = this_min_span;
        }
    }
    return min_maxspan;
}

//计算拥挤距离
void crowdDistance(int now_rank){
    vector<Individual>::iterator iter;
    Individual front_individuals[MAXN];
    int length = 0;
    double max_communication = get_max_communication(now_rank);
    double min_communication = get_min_communication(now_rank);
    double max_maxspan = get_max_maxspan(now_rank);
    double min_maxspan = get_min_maxspan(now_rank);
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter){
        front_individuals[length] = (*iter);
        length ++;
    }
    printf("length = %d\n", length);
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++){
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].communication_cost - front_individuals[i-1].communication_cost) / (max_communication - min_communication);
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp1);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++){
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].maxspan - front_individuals[i-1].maxspan) / (max_communication - min_communication);
    }

    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp2);
//    printf("*************\n");
//    for(int i = 0 ; i < n ; i ++){
//        printf("distance = %.2lf\n", front_individuals[i].crowd_distance);
//    }
//    printf("*************\n");
    Front[now_rank].erase(Front[now_rank].begin(), Front[now_rank].end());
    for(int i = 0 ; i < length ; i ++){
        Front[now_rank].push_back(front_individuals[i]);
    }
}

void make_new_pop(Individual individuals[], int length){

    vector<int>::iterator iter;
    int flag_individual[MAXN]; //标记这个个体是否被选择过
    //tournament_selection
    memset(flag_individual, 0, sizeof(flag_individual)); //初始全部未被选择
    for(int i = 0 ; i < length ; i ++){
        int target1 = rand() % ((length<<1));
        while(flag_individual[target1] != 0){
            target1 = rand() % ((length<<1));
        }
        flag_individual[target1] = 1;
        int target2 = rand() % ((length<<1));
        while(flag_individual[target2] != 0){
            target2 = rand() % ((length<<1));
        } //随机选择两个目标
        flag_individual[target2] = 1;
//        printf("target1 = %d, target2 = %d\n", target1, target2);
        int temp = rand() % 10;
        gacrossover(target1, target2, &new_individual);

        if(temp >= 0 && temp < 9) {}
        else {
            gamutation(&new_individual);
        }
//        printf("新生成机器11\n");
//        for(int j = 0 ; j < m ; j ++){
//            printf("第%d个机器: ", j);
//            for(iter = new_individual.machine[j].begin(); iter != new_individual.machine[j].end() ; iter ++){
//                printf("%d ", (*iter));
//            }
//            printf("\n");
//        }

        copy_individual(&individuals[length + i], &new_individual);

//        gacrossover(target1, target2, &new_individual);
//        gamutation(&new_individual);


    }
}

double get_min_time_in_array(double arr[]){
    double min_time = inf;
    int min_id;
    for(int i = 0 ; i < m ; i ++){
        if(min_time > arr[i]){
            min_time = arr[i];
            min_id = i;
        }
    }
    return min_time;
}

double get_min_id_in_array(double arr[]){
    double min_time = inf;
    int min_id;
    for(int i = 0 ; i < m ; i ++){
        if(min_time > arr[i]){
            min_time = arr[i];
            min_id = i;
        }
    }
    return min_id;
}

void greedy_for_workload(){
    double now_span[MAXN];
    memset(now_span, 0, sizeof(now_span));
    for(int i = 0 ; i < n ; i ++){
        time_in_machine[i].id = i;
        time_in_machine[i].time = t[0][i];
    }
    qsort(time_in_machine, n, sizeof(time_in_machine[0]), cmp3);
    for(int i = 0 ; i < n ; i ++){
        if(i >= 0 && i < m){
            Collection[0].machine[i].push_back(time_in_machine[i].id);
            now_span[i] += time_in_machine[i].time;
        }else{
            int pos = get_min_id_in_array(now_span);
            Collection[0].machine[pos].push_back(time_in_machine[i].id);
            now_span[pos] += time_in_machine[i].time;
        }
    }
}

int father[MAXN];
/* rank[x]表示x的秩 */
int rank[MAXN];
/* 初始化集合 */
void Make_Set(int x)
{
	father[x] = x;
	rank[x] = 0;
}
/* 查找x元素所在的集合,回溯时压缩路径 */
int Find_Set(int x)
{
	if (x != father[x])
	{
		father[x] = Find_Set(father[x]);
	}
	return father[x];
}
/* 按秩合并x,y所在的集合 */
void Union(int x, int y)
{
	x = Find_Set(x);
	y = Find_Set(y);
	if (x == y) return;
	if (rank[x] > rank[y])
	{
		father[y] = x;
	}
	else
	{
		if (rank[x] == rank[y])
		{
			rank[y]++;
		}
		father[x] = y;
	}
}

void greedy_for_communication(){
    vector<int>::iterator iter;
    bool flag;
    int machine_number[MAXN];
    memset(machine_number, 0, sizeof(machine_number));
    int max_number = n / m;
    for(int i = 0 ; i < n ; i ++){
        flag = false;
        for(int j = 0 ; j < m ; j ++){
            for(iter = Collection[1].machine[j].begin(); iter != Collection[1].machine[j].end(); ++ iter){

                if(c[i][(*iter)] > 0 && machine_number[j] <= max_number){
                    Collection[1].machine[j].push_back(i);
                    machine_number[j] ++;
                    flag = true;
                    break;
                }
            }
            if(flag) break;
        }
        if(!flag){
            int pos;
            int minn = inf;
            for(int k = 0 ; k < m ; k ++){
                if(machine_number[k] < minn && machine_number[k] <= max_number){
                    minn = machine_number[k];
                    pos = k;
                }
            }
            Collection[1].machine[pos].push_back(i);
            machine_number[pos] ++;
        }
    }
//        Collection[1].machine[0].push_back(5);
//        Collection[1].machine[1].push_back(2);
//        Collection[1].machine[1].push_back(3);
//        Collection[1].machine[1].push_back(6);
//        Collection[1].machine[2].push_back(1);
//        Collection[1].machine[2].push_back(10);
//        Collection[1].machine[2].push_back(0);
//        Collection[1].machine[3].push_back(4);
//        Collection[1].machine[4].push_back(13);
//        Collection[1].machine[5].push_back(8);
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

void init(){

    set<int> flag_machine;
    stack<int> segment;
    set<int> interval;
    int kk, kkk;
    vector<int>::iterator iter;
    set<int>::iterator iiter;
    int tempkk;
    for(int i = 0 ; i < pop*2 ; i ++){
        for(int j = 0 ; j < m ; j ++){
            Collection[i].machine[j].erase(Collection[i].machine[j].begin(), Collection[i].machine[j].end());
        }
    }

    greedy_for_workload();
    greedy_for_communication();

    for(int i = 2 ; i < pop*2 ; i ++){

        flag_machine.clear();
        while(!segment.empty()){
            segment.pop();
        }

        for(int j = 0 ; j < n * 2 ; j ++){
            int temp = rand() % n;
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
            int temp = rand() % (n);
            if(temp == 0) temp = 1;
            while(!interval.insert(temp).second){
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
//        printf("\n");
//        getchar();
    }
}

//Main Process
void solve(){

    scanf("%d%d", &pop, &gen);
    for(int i = 0 ; i < m ; i ++){
        for(int j = 0 ; j < n ; j ++){
            scanf("%lf", &t[i][j]);
        }
    }
    for(int i = 0 ; i < n ; i ++){
        for(int j = 0 ; j < n ; j ++){
            scanf("%lf", &c[i][j]);
        }
    }


    int t = 0;
    init();

    while(t < gen){
//        printf("t=%d\n", t);

        int P_size = 0;
        int now_rank = 1;

        make_new_pop(Collection, pop);

        for(int i = 0 ; i < pop*2 ; i ++){
            evaluate_objective(&Collection[i]);
        }

        non_domination_sort(Collection, pop * 2);
        while(1){
            if(P_size + Front[now_rank].size() > pop){
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter){
                Collection[P_size] = (*iter);
                P_size ++;
            }
            now_rank ++;
        }
        crowdDistance(now_rank);
        while(1){
            if(P_size > pop){
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter){
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
    int tot = 0;
    for(int i = 0 ; i < pop ; i ++){
//        if(Collection[i].front == 1){
            printf("[");
            printf("%.2lf,", Collection[i].maxspan);
            printf("%.2lf", Collection[i].communication_cost, Collection[i].front);
            printf("],", Collection[i].front);
            tot ++;
//        }
    }
    printf("tot = %d\n", tot);
}


int main(){
    srand(3);
    freopen("in.txt", "r", stdin);
    freopen("out3.txt", "w", stdout);
    solve();
    return 0;
}
