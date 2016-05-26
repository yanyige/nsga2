#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<vector>

using namespace std;

const int SIZEOFDV = 2000;

//*******
//**pop - Population size
//**gen - Total number of generations
int pop, gen;


//*******
//**number_of_objectives - the number of objective function
//**number_of_decision_variables - number of decision variables
//**min_range_of_decision_variable[] - minimum possible value for each decision variable
//**max_range_of_decision_variable[] - maximum possible value for each decision variable

int number_of_objectives, number_of_decision_variables, min_range_of_decision_variable[SIZEOFDV], max_range_of_decision_variable[SIZEOFDV];


//*******
//**Ŀ�꺯��   Minimize communication cost
//**Ŀ�꺯��   Minimize the balance of processor workload


//*******
//**input
//**t(m,n) Execution time of a task on a processor
//**c(m,n) Communication cost between two tasks

double t[SIZEOFDV][SIZEOFDV];
double c[SIZEOFDV][SIZEOFDV];

struct Individual{
    int id;
    vector< vector<int> > machine; //save permutation of the tasks
    double communication_cost; //object 1
    double maxspan; //object 2
    int front; //rank of domination
    vector< Individual > S; //the collections dominated by this Individual
    int n;// count of dominated solution
}Collection[100];



void evaluate_objective(Individual *i){
    vector< vector<int> >::iterator iter;
    vector<int>:: iterator jter;
    i->maxspan = 0;
    i->communication_cost = 0;
    for(iter = i->machine.begin(); iter != i->machine.end(); ++ iter){
        double span = 0;
        for(jter = (*iter).begin(); jter != (*iter).end(); ++ jter){
            span += t[i->id][(*jter)];
            if(c[i->id][(*jter)] > 0){
                i->communication_cost += c[i->id][(*jter)];
            }
        }
        if(span > i->maxspan){
            i->maxspan = span;
        }
    }
}

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
        //fuzhi
    }
}

int main(){

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



