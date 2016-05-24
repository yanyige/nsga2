#include<stdio.h>
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

struct Individual{
    vector<int> machine[10];
};

int main(){

    printf("NSGA-II: Please enter the population size and number of generations as input arguments.\npopulation size: ");
    scanf("%d", &pop);
    printf("number of generations: ");
    scanf("%d", &gen);

    printf("Input the number of objective: ");
    scanf("%d", &number_of_objectives);
    printf("Input the number of decision variables: ");
    scanf("%d", &number_of_decision_variables);

    for(int i = 1; i <= number_of_decision_variables; i ++){
        printf("Input the minimum value for decision variable ");
        scanf("%d", min_range_of_decision_variable[i]);
        printf("Input the maximum value for decision variable ");
        scanf("%d", max_range_of_decision_variable[i]);
    }

    //initial function 初始化一个解


    return 0;
}
