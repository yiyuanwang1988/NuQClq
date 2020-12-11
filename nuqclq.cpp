#include "nuqclq.h"

int randInTieMax(int* candidate, int candidate_size, int* target1, int* target2)
{
    int max_target1 = INT_MIN, max_target2 = INT_MIN, u;
    tie_array_size = 0;
    if(target2 == NULL)
        target2 = target1;
    for(int i = 0; i < candidate_size; ++i)
    {
        u = candidate[i];
        if(max_target1 < target1[u] || (max_target1 == target1[u] && max_target2 < target2[u]))
        {
            max_target1 = target1[u];
            max_target2 = target2[u];
            tie_array_size = 1;
            tie_array[0] = u;
        }
        else if(max_target1 == target1[u] && max_target2 == target2[u])
            tie_array[tie_array_size++] = u;
    }
    if(tie_array_size == 0)
        return -1;
    return tie_array[rand() % tie_array_size];
}

int randInTieMin(int* candidate, int candidate_size, int* target1, int* target2)
{
    int min_target1 = INT_MAX, min_target2 = INT_MAX, u;
    tie_array_size = 0;
    if(target2 == NULL)
        target2 = target1;
    for(int i = 0; i < candidate_size; ++i)
    {
        u = candidate[i];
        if(min_target1 > target1[u] || (min_target1 == target1[u] && min_target2 > target2[u]))
        {
            min_target1 = target1[u];
            min_target2 = target2[u];
            tie_array_size = 1;
            tie_array[0] = u;
        }
        else if(min_target1 == target1[u] && min_target2 == target2[u])
            tie_array[tie_array_size++] = u;
    }
    if(tie_array_size == 0)
        return -1;
    return tie_array[rand() % tie_array_size];
}


void addVertex(int v_aim)
{
    int u, v, index;
    is_v_in_sol[v_aim] = true;
    cur_sol_quality += profit[v_aim];
    cur_sol[cur_sol_size++] = v_aim;
    freq[v_aim]++;
    if(neighbor_s_index[v_aim] != -1)
    {
        u = neighbor_s[--neighbor_s_size];
        index = neighbor_s_index[v_aim];
        neighbor_s[index] = u;
        neighbor_s_index[u] = index;
        neighbor_s_index[v_aim] = -1;
    }
    cc_size[v_aim] = 0;
    for(int i = 0; i < degree[v_aim]; ++i)
    {
        u = neighbor[v_aim][i];
        profit[u]++;
        if(is_v_in_sol[u])
        {
            if(cc[v_aim] > cc_thre[v_aim])
                cc_size[u]--;
            continue;
        }
        if(neighbor_s_index[u] == -1)
        {
            neighbor_s[neighbor_s_size] = u;
            neighbor_s_index[u] = neighbor_s_size;
            neighbor_s_size++;
        }
        if(cc[u] > cc_thre[u])
            cc_size[v_aim]++;
        else if(cc[u] == cc_thre[u])
        {
            for(int j = 0; j < degree[u]; ++j)
            {
                v = neighbor[u][j];
                if(is_v_in_sol[v])
                    cc_size[v]++;
            }
        }
        if(cc[u] <= Max_degree)
            cc[u]++;
    }
    cc_thre[v_aim] = (cc_thre[v_aim] + 1) % 7;
    //if(cc_thre)
    //cc_thre[v_aim]++;
}

void removeVertex(int u_aim)
{
    int u, v, index;
    is_v_in_sol[u_aim] = false;
    cur_sol_quality -= profit[u_aim];
    freq[u_aim]++;
    for(int i = 0; i < cur_sol_size; ++i)
        if(cur_sol[i] == u_aim)
        {
            cur_sol[i] = cur_sol[--cur_sol_size];
            break;
        }
    cc[u_aim] = 0;
    if(profit[u_aim] > 0)
    {
        neighbor_s_index[u_aim] = neighbor_s_size;
        neighbor_s[neighbor_s_size++] = u_aim;
    }

    for(int i = 0; i < degree[u_aim]; ++i)
    {
        u = neighbor[u_aim][i];
        profit[u]--;
        if(profit[u] == 0 && neighbor_s_index[u] >= 0)
        {
            v = neighbor_s[--neighbor_s_size];
            index = neighbor_s_index[u];
            neighbor_s[index] = v;
            neighbor_s_index[v] = index;
            neighbor_s_index[u] = -1;
        }
    }
}

int initSolution()
{
    int v_aim, u, edge_in_complete;
    cur_sol_quality = 0;
    cur_sol_size = 0;
    neighbor_s_size = 0;
    memset(is_v_in_sol, 0, v_num * sizeof(bool));
    memset(profit, 0, v_num * sizeof(int));
    memset(neighbor_s_index, -1, v_num * sizeof(int));
    memset(cc_size, 0, v_num * sizeof(int));
    memset(subscore, 0, v_num * sizeof(int));
    memset(cc_thre, 0, v_num * sizeof(int));
    int flag = 0;
    for(int i = 0; i < v_num; ++i)
    {
        cc[i] = cc_thre[i] + 1;
        temp_array[i] = i;
        if(freq[i] > 100000000)
            flag = 1;
    }
    if(flag == 1)
        for(int i = 0; i < v_num; ++i)
            freq[i] = freq[i] >> 1;

    edge_in_complete = 0;
    v_aim = randInTieMin(temp_array, v_num, freq, NULL); // first vertex with min frequency;
    addVertex(v_aim);
    while(cur_sol_size < v_num)
    {
        if(neighbor_s_size > 0)
        {
            if((double)rand() / (double)RAND_MAX < 0.8)
                v_aim = randInTieMin(neighbor_s, neighbor_s_size, freq, NULL);
            else
                v_aim = neighbor_s[rand() % neighbor_s_size];
        }
        else
        {
            temp_array_size = 0;
            for(int i = 0; i < v_num; ++i)
                if(!is_v_in_sol[i])
                    temp_array[temp_array_size++] = i;
            v_aim = temp_array[rand() % temp_array_size];
        }
        if(cur_sol_quality + profit[v_aim] < ceil((double)(edge_in_complete + cur_sol_size) * param_k))
            break;
        edge_in_complete += cur_sol_size;
        addVertex(v_aim);
    }
    if(!checkSol(cur_sol, cur_sol_size))
    {
        printf("###Error in initSolution: check init solution\n");
        return 1;
    }
    //if(cur_sol_size > opt_sol_size || (cur_sol_size == opt_sol_size && cur_sol_quality > opt_sol_quality))
    if(cur_sol_size > opt_sol_size)
        updateSol();
    for(int i = 0; i < neighbor_s_size; ++i)
    {
        u = neighbor_s[i];
        subscore[u] = profit[u];
    }
    for(int i = 0; i < cur_sol_size; ++i)
    {
        u = cur_sol[i];
        cc_size[u] = degree[u] - profit[u];
    }
    return 0;
}

void updateSol()
{
    opt_sol_size = cur_sol_size;
    opt_sol_quality = cur_sol_quality;
    for(int i = 0; i < cur_sol_size; ++i)
        opt_sol[i] = cur_sol[i];
    search_time = getUtime();
}

bool checkSol(int* sol, int sol_size)
{
    memset(temp_array, 0, v_num * sizeof(int));
    for(int i = 0; i < sol_size; ++i)
        temp_array[sol[i]] = 1;
    int quality = 0;
    int u, v;
    for(int i = 0; i < sol_size; ++i)
    {
        u = sol[i];
        for(int j = 0; j < degree[u]; ++j)
        {
            v = neighbor[u][j];
            if(temp_array[v])
                quality++;
        }
    }
    quality = quality / 2;
    if((double)quality / (double)((sol_size - 1) * sol_size / 2) < param_k)
        return false;
    return true;
}

int searchInAdd(int &v_aim)
{
    int u, v, w, count;
    bool no_cc = false;
    temp_array_size = 0;
    if(neighbor_s_size == 0)
    {
        if(v_num > cur_sol_size * 10)
        {
            do
            {
                v_aim = rand() % v_num;
            }while(is_v_in_sol[v_aim]);
        }
        else
        {
            for(int i = 0; i < v_num; ++i)
                if(!is_v_in_sol[i])
                    temp_array[temp_array_size++] = i;
            v_aim = temp_array[rand() % temp_array_size];
        }
        return 0;
    }
    u = rand() % cur_sol_size;
    for(count = u; count < u + cur_sol_size; ++count)
    {
        v = cur_sol[count % cur_sol_size];
        if(cc_size[v] > 0)
            break;
        if(degree[v] > profit[v])
            w = v;
    }
    if(count == u + cur_sol_size)
    {
        v = w;
        no_cc = true;
    }

    for(int i = 0; i < degree[v]; ++i)
    {
        u = neighbor[v][i];
        if(is_v_in_sol[u])
            continue;
        if(cc[u] > cc_thre[u] || no_cc)
            temp_array[temp_array_size++] = u;
    }

    if(temp_array_size == 0)
    {
        printf("###Error in searchInAdd ");
    }

    v_aim = randInTieMax(temp_array, temp_array_size, profit, subscore);
    //v_aim = randInTieMax(temp_array, temp_array_size, profit, NULL);
    return 0;
}

int searchInDrop(int &u_aim, int tabu_v)
{
    int tmp1 = 0, tmp2 = 0;
    if(Density > 0.5)
    {
        u_aim = randInTieMin(cur_sol, cur_sol_size - 1, profit, subscore);
        //u_aim = randInTieMin(cur_sol, cur_sol_size - 1, profit, NULL);
        return 0;
    }
    int count = cur_sol_size >> 1;
    if(count == 0)
        return -1;
    int u;
    temp_array_size = 0;
    while(count > 0)
    {
        u = cur_sol[rand() % cur_sol_size];
        if(u == tabu_v)
            continue;
        temp_array[temp_array_size++] = u;
        count--;
    }
    if(temp_array_size == 0)
        printf("###Error in searchInDrop\n");

    u_aim = randInTieMin(temp_array, temp_array_size, profit, subscore);
    //u_aim = randInTieMin(temp_array, temp_array_size, profit, NULL);
    return 0;
}

int searchQuasi()
{
    int no_expand_iter, v_aim, u_aim, u, v, rn;
    int edge_in_complete, edge_in_quasi;
    int tabu_v;
    while(getUtime() <= Cuff_time)
    {
        if(initSolution() != 0)
            return 1;
        no_expand_iter = 0;
        edge_in_complete = cur_sol_size * (cur_sol_size - 1) >> 1;
        edge_in_quasi = ceil(double(edge_in_complete) * param_k);
        while(no_expand_iter < Max_iter)
        {
            if(cur_sol_quality >= edge_in_quasi)
            {
                //if(cur_sol_size > opt_sol_size || (cur_sol_size == opt_sol_size && cur_sol_quality > opt_sol_quality))
                if(cur_sol_size > opt_sol_size)
                    updateSol();
                if(cur_sol_size == v_num)
                {
                    printf("### find the opt solution: |S| = the number of vertex in G\n");
                    return 0;
                }
                edge_in_complete += cur_sol_size;
                edge_in_quasi = ceil(double(edge_in_complete) * param_k);
                rn = searchInAdd(v_aim);
                addVertex(v_aim);
                subscore[v_aim] = 0;
                tabu_v = v_aim;
                no_expand_iter = 0;
                continue;
            }
            rn = searchInAdd(v_aim);
            addVertex(v_aim);
            subscore[v_aim] = 0;
            tabu_v = v_aim;
            rn = searchInDrop(u_aim, tabu_v);
            removeVertex(u_aim);
            subscore[u_aim] = 0;
            for(int i = 0; i < neighbor_s_size; ++i)
            {
                u = neighbor_s[i];
                subscore[u] += profit[u];
            }
            for(int i = 0; i < cur_sol_size; ++i)
            {
                u = cur_sol[i];
                subscore[u] = subscore[u] - (Max_degree - (degree[u] - profit[u]));
            }
            no_expand_iter++;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    char* file_name;
    int seed;
    file_name = argv[1];
    Cuff_time = atoi(argv[2]);
    param_k = atof(argv[3]);
    seed = atoi(argv[4]);
    srand(seed);
    Max_iter = 4000;
    Tabu_value = 7;
    if(readGraph(file_name) != 0)
    {
        free(degree);
        for(int i = 0; i < v_num; ++i)
            free(neighbor[i]);
        free(neighbor);
            return 0;
    }
    read_time = getUtime();
    //printf("read time:%lfs  ", read_time);
    initVariable();
    int rn = searchQuasi();
    //printf("%s, solSize:%d, time:%lfs\n", file_name, opt_sol_size, search_time);
    char* file_name_temp = strrchr(file_name, '/');
    if(file_name_temp == NULL)
        file_name_temp = file_name;
    printf("%s %lf %d %d %lf\n", file_name_temp, param_k, seed, opt_sol_size, search_time - read_time);
    if(!checkSol(opt_sol, opt_sol_size))
        printf("###the solution is wrong!\n");
    freeAll();
    return 0;
}
