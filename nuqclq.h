/*********************************************************
 ** This is a local search solver for Maximum Quasi-Clique Problem.
 *********************************************************/

 /*********************************************************
 ** Date: 2020.8.25
 ** NuQClq
 ** Author: Jiejiang Chen

 *********************************************************/

 /*********************************************************
 ** NuQClq Version 2020.8.25
 *********************************************************/

#include<sstream>
#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<math.h>
#include<float.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/resource.h>

using namespace std;

struct EDGE
{
    int v1;
    int v2;
};

int v_num, e_num;
double Density;
EDGE* edge;
int* degree;
int** neighbor;

int* cur_sol;
int cur_sol_size;
int cur_sol_quality;

int* opt_sol;
int opt_sol_size;
int opt_sol_quality;

bool* is_v_in_sol;

int* tie_array;
int tie_array_size;

int* temp_array;
int temp_array_size;

int* cc;
int* cc_size;
int* cc_thre;
int* freq;

int* aux_array;
int* aux_array_index;
int aux_array_size;

int* profit;
int* subscore;

int* neighbor_s;
int* neighbor_s_index;
int neighbor_s_size;

double param_k;
int Cuff_time = 1000;
int Max_iter = 1000;
int Tabu_value = 7;
int Max_degree;

double read_time, init_time, search_time;

void initVariable();
void freeAll();
int readGraph(char* file_name);
void initClique();
int initSolution();
void updateSol();
bool checkSol(int* sol, int sol_size);
double getUtime();

double getUtime()
{
    struct rusage utime;
    getrusage(RUSAGE_SELF, &utime);
    return (double)(utime.ru_utime.tv_sec + (double)utime.ru_utime.tv_usec / 1000000);
}

void initVariable()
{
    cur_sol = (int*)malloc(v_num * sizeof(int));
    cur_sol_size = 0;
    cur_sol_quality = 0;
    opt_sol = (int*)malloc(v_num * sizeof(int));
    opt_sol_size = 0;
    opt_sol_quality = 0;
    is_v_in_sol = (bool*)malloc(v_num * sizeof(bool));
    tie_array = (int*)malloc(v_num * sizeof(int));
    tie_array_size = 0;
    aux_array = (int*)malloc(v_num * sizeof(int));
    aux_array_index = (int*)malloc(v_num* sizeof(int));
    aux_array_size = 0;
    profit = (int*)malloc(v_num * sizeof(int));
    temp_array = (int*)malloc(v_num * sizeof(int));
    temp_array_size = 0;
    cc = (int*)malloc(v_num * sizeof(int));
    cc_size =(int*)malloc(v_num * sizeof(int));
    cc_thre = (int*)malloc(v_num * sizeof(int));
    for(int i = 0; i < v_num; ++i)
        cc_thre[i] = degree[i] * param_k / 2.0;
    freq = (int*)malloc(v_num * sizeof(int));
    subscore = (int*)malloc(v_num * sizeof(int));
    neighbor_s = (int*)malloc(v_num * sizeof(int));
    neighbor_s_index = (int*)malloc(v_num * sizeof(int));
    memset(freq, 0, v_num * sizeof(int));
}

void freeAll()
{
    free(cur_sol);
    free(opt_sol);
    free(is_v_in_sol);
    free(tie_array);
    free(aux_array);
    free(aux_array_index);
    free(profit);
    free(temp_array);
    free(cc);
    free(cc_size);
    free(cc_thre);
    free(freq);
    free(subscore);
    free(neighbor_s);
    free(neighbor_s_index);
    free(degree);
    for(int i = 0; i < v_num; ++i)
        free(neighbor[i]);
    free(neighbor);
}

int readGraph(char* file_name)
{
    int u, v;
    int count = 0;
    char temp1[10], temp2[10];
    ifstream ffs;

    ffs.open(file_name);
    if(ffs.fail())
    {
        printf("### Error Open, File Name:%s\n", file_name);
        return 1;
    }
    ffs >> temp1 >> temp2 >> v_num >> e_num;
    neighbor = (int**)malloc(v_num * sizeof(int*));
    degree = (int*)malloc(v_num * sizeof(int));
    edge = (EDGE*)malloc(e_num * sizeof(EDGE));
    memset(degree, 0, v_num * sizeof(int));
    while(ffs >> temp1>> u >> v)
    {
        u--;
        v--;
        degree[u]++;
        degree[v]++;
        edge[count].v1 = u;
        edge[count].v2 = v;
        count++;
    }

    Density = 2.0 * e_num / (v_num - 1) / v_num;
    Max_degree = 0;
    for(int i = 0; i < v_num; ++i)
    {
        neighbor[i] = (int*)malloc(degree[i] * sizeof(int));
        if(degree[i] > Max_degree)
            Max_degree = degree[i];
        degree[i] = 0;
    }
    for(int i = 0; i < e_num; ++i)
    {
        u = edge[i].v1;
        v = edge[i].v2;
        neighbor[u][degree[u]++] = v;
        neighbor[v][degree[v]++] = u;
    }
    free(edge);
    ffs.close();
    return 0;
}
