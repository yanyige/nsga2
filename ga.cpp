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
int pop, gen;

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

int cmp4(const void *a, const void *b){
    struct Individual *c = (Individual *)a; struct Individual *d = (Individual *)b;
    if(c->front != d->front)
              return c->front - d->front;
    else
              return d->maxspan - c->maxspan;
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
}

// 非支配排序
int non_domination_sort(Individual individuals[], int length){
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
    return rank;
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
        for(int j = 0 ; j < interval[i] ; j ++){
            individual->machine[i].push_back(segment[temp]);
            temp ++;
        }
    }

}

int round_robin_selection(Individual individuals[], int length){
    int target = 0;
    double total_fitness = 0;
    for(int i = 0 ; i < length ; i ++){
        total_fitness = total_fitness + individuals[i].front;
    }
    double dRange = (((rand()+ rand())%100001)/(100000 + 0.0000001)) * total_fitness;
    double dCursor = 0;
    for(int i = 0 ; i < length ; i ++){
        dCursor = dCursor + individuals[i].front;
        target ++;
        if(dCursor > dRange){
            break;
        }
    }
    return target;
}

int compare_individual(Individual individual1, Individual individual2){
    evaluate_objective(&individual1);
    evaluate_objective(&individual2);
    if(individual1.communication_cost > individual2.communication_cost && individual1.maxspan > individual2.maxspan) return 2;
    else if(individual1.communication_cost < individual2.communication_cost && individual1.maxspan < individual2.maxspan) return 1;
    else return 3;
}

void make_new_pop(Individual individuals[], int length){

    vector<int>::iterator iter;
    int flag_individual[MAXN]; //标记这个个体是否被选择过
    //tournament_selection
    memset(flag_individual, 0, sizeof(flag_individual)); //初始全部未被选择
    for(int i = 0 ; i < length ; i ++){

        int target1 = round_robin_selection(individuals, length);
        int target2 = round_robin_selection(individuals, length);
        while(target1 == target2){
            target2 = round_robin_selection(individuals, length);
        }
        int temp = rand() % 10;
        gacrossover(target1, target2, &new_individual);

        if(temp >= 0 && temp < 9) {}
        else {
            gamutation(&new_individual);
        }
        if(compare_individual(individuals[target1], individuals[target2]) == 1){
            if(compare_individual(individuals[target1], new_individual) == 1){
                individuals[length + i] = individuals[target1];
            }else if(compare_individual(individuals[target1], new_individual) == 2){
                individuals[length + i] = new_individual;
            }else{
                int temp = rand() % 2;
                if(temp){
                    individuals[length + i] = new_individual;
                }else{
                    individuals[length + i] = individuals[target1];
                }
            }
        }else if(compare_individual(individuals[target1], individuals[target2]) == 2){
            if(compare_individual(individuals[target2], new_individual) == 1){
                individuals[length + i] = individuals[target2];
            }else if(compare_individual(individuals[target2], new_individual) == 2){
                individuals[length + i] = new_individual;
            }else{
                int temp = rand() % 2;
                if(temp){
                    individuals[length + i] = new_individual;
                }else{
                    individuals[length + i] = individuals[target2];
                }
            }
        }else{
            int temp = rand() % 2;
            if(temp){
                if(compare_individual(individuals[target1], new_individual) == 1){
                    individuals[length + i] = individuals[target1];
                }else if(compare_individual(individuals[target1], new_individual) == 2){
                    individuals[length + i] = new_individual;
                }else{
                    int temp = rand() % 2;
                    if(temp){
                        individuals[length + i] = new_individual;
                    }else{
                        individuals[length + i] = individuals[target1];
                    }
                }
            }else{
                if(compare_individual(individuals[target2], new_individual) == 1){
                    individuals[length + i] = individuals[target2];
                }else if(compare_individual(individuals[target2], new_individual) == 2){
                    individuals[length + i] = new_individual;
                }else{
                    int temp = rand() % 2;
                    if(temp){
                        individuals[length + i] = new_individual;
                    }else{
                        individuals[length + i] = individuals[target2];
                    }
                }
            }
        }

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
        for(int j = 0 ; j < m ; j ++){
            Collection[i].machine[j].erase(Collection[i].machine[j].begin(), Collection[i].machine[j].end());
        }
    }

    greedy_for_workload();
    greedy_for_communication();

    for(int i = 2 ; i < pop ; i ++){

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

        int P_size = 0;
        int now_rank = 1;
        int tot_rank;
        printf("t=%d\n", t);
//        make_new_pop(Collection, pop);

        for(int i = 0 ; i < pop ; i ++){
            evaluate_objective(&Collection[i]);
        }

        tot_rank = non_domination_sort(Collection, pop);

        qsort(Collection, pop, sizeof(Collection[0]), cmp4);
        printf("Collection[0]maxspan = %.2lf\n", Collection[0].maxspan);
        make_new_pop(Collection, pop);

        for(int i = 0 ; i < pop ; i ++){
            Collection[i] = Collection[i + pop];
        }

        t ++;
    }
    for(int i = 0 ; i < pop ; i ++){
        evaluate_objective(&Collection[i]);
    }
    non_domination_sort(Collection, pop);
    int tot = 0;
    for(int i = 0 ; i < pop ; i ++){
        if(Collection[i].front == 1){
            printf("[");
            printf("%.2lf,", Collection[i].maxspan);
            printf("%.2lf", Collection[i].communication_cost, Collection[i].front);
            printf("],", Collection[i].front);
            tot ++;
        }
    }
    printf("tot = %d\n", tot);
}


int main(){
    srand(4);
    freopen("./ga/in.txt", "r", stdin);
//    freopen("./ga/out.txt", "w", stdout);
    solve();
    return 0;
}
