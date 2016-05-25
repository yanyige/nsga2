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
//**目标函数   Minimize communication cost
//**目标函数   Minimize the balance of processor workload


//*******
//**input
//**t(m,n) Execution time of a task on a processor
//**c(m,n) Communication cost between two tasks

double t[SIZEOFDV][SIZEOFDV];
double c[SIZEOFDV][SIZEOFDV];

struct Individual{
    vector< vector<int> > machine; //save permutation of the tasks
    double communication_cost;
    double maxspan;
};



void evaluate_objective(Individual *i){
    vector< vector<int> >::iterator iter;
    vector<int>:: iterator jter;
    i->maxspan = 0;
    i->communication_cost = 0;
    int k = 0;
    for(iter = i->machine.begin(); iter != i->machine.end(); ++ iter){
        double span = 0;
        for(jter = (*iter).begin(); jter != (*iter).end(); ++ jter){
            span += t[k][(*jter)];
            if(c[k][(*jter)] > 0){
                i->communication_cost += c[k][(*jter)];
            }
        }
        if(span > i->maxspan){
            i->maxspan = span;
        }
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



