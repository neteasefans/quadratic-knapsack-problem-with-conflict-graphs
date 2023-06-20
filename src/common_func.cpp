#include"func_state.h"
#include"global_variables.h"

instance_data Ins;
solution_data Sol_best;
solution_data Sol_cur;		
solution_data Sol_best_g;	
int *Delta_matrix;
int Runs;
neighborhood_data *Neighbors;
int *Frequency_arr;				 

int Non_imp_iters;
int Pop_size;
solution_data *Pop;
solution_data Child;
solution_data Child_oppo;

double Shake_str;				
int Shake_non_imp;				

int *Tabu_list;
int Tabu_tenure;	
double Start_time, Run_time, Time_limit;


void read_instance(char *instance_name)
{
	ifstream fin;
	fin.open(instance_name);
	if (fin.fail())
	{
		cout << "### error open, instance_name " << instance_name << endl;
		exit(0);
	}
	fin >> Ins.num_v;
	fin >> Ins.num_edges;
	fin >> Ins.capacity;
	Ins.weight = new int[Ins.num_v];
	Ins.profit = new int[Ins.num_v];
	int count_v = 0;
	while (count_v < Ins.num_v)
		fin >> Ins.profit[count_v++];
	count_v = 0;
	while (count_v < Ins.num_v)
		fin >> Ins.weight[count_v++];

	count_v = 0;
	int total_nums = Ins.num_v * (Ins.num_v - 1) / 2;
	int first_v = 0;
	int second_v = first_v + 1;
	int line_cnt = 0;
	
	//read quadratic terms 
			
	Ins.quad_profit = new int*[Ins.num_v];
	for(int i=0;i<Ins.num_v;i++)
		Ins.quad_profit[i]= new int[Ins.num_v];
	for(int i=0;i<Ins.num_v;i++)
		for(int j=0;j<Ins.num_v;j++)
			Ins.quad_profit[i][j] =0;
	while (count_v++ < total_nums)
	{
		int quad_pro;
		fin >> quad_pro;
		Ins.quad_profit[first_v][second_v] = quad_pro;
		Ins.quad_profit[second_v][first_v] = quad_pro;
		second_v++;
		if(second_v == Ins.num_v)
		{
			first_v++;
			second_v= first_v + 1;
			line_cnt++;
			//cout<<"line_cnt = "<< line_cnt<< ", profit="<<quad_pro<< ",first_v = "<< first_v << ",sec_v="<< second_v<<endl;			
		}			
	}
	
	//read edges
	/*Ins.edges=new int*[Ins.num_v];
	for(int i=0; i<Ins.num_v;i++)
		Ins.edges[i]= new int[Ins.num_v];
	for(int i=0;i<Ins.num_v;i++)
		for(int j=0;j<Ins.num_v;j++)
			Ins.edges[i][j] =0;
	while(!fin.eof())
	{
		fin>>first_v>>second_v;
		Ins.edges[first_v -1][second_v -1] =1;
		Ins.edges[second_v -1][first_v -1] =1;
	}*/
	//read adjacent table
	Ins.adj_table=new int*[Ins.num_v];
	for(int i=0; i<Ins.num_v;i++)
		Ins.adj_table[i]= new int[Ins.num_v];
	Ins.adj_len=new int[Ins.num_v];
	for(int i=0;i<Ins.num_v;i++)
		Ins.adj_len[i]=0;
	while(!fin.eof())
	{
		fin>>first_v>>second_v;	
		Ins.adj_table[first_v -1][Ins.adj_len[first_v -1]++]=second_v -1;
		Ins.adj_table[second_v -1][Ins.adj_len[second_v -1]++]=first_v -1;
	}
	
	cout << "finished read" << ", num_v=" << Ins.num_v <<", num_edges="<< Ins.num_edges<<", cap="<<Ins.capacity << endl;
	fin.close();
	
	
#ifdef DEBUG_READ_INSTANCE1
	cout << "num_v=" << Ins.num_v << endl;
	cout << "num_edges=" << Ins.num_edges << endl;
	cout << "capacity=" << Ins.capacity << endl;
	
	cout << "profit=" << endl;	
	for (int i = 0; i < Ins.num_v; i++)
		cout << Ins.profit[i] << " ";
		
	cout << endl<< "weight=" << endl;
	for (int i = 0; i < Ins.num_v; i++)
		cout << Ins.weight[i] << " ";
	getchar();
	cout << endl << ", quad_profit=" << endl;;
	for (int i = 0; i < Ins.num_v; i++)
	{
		for (int j = i+1; j < Ins.num_v; j++)
		{
			cout << Ins.quad_profit[i][j] << " ";
			if(Ins.quad_profit[i][j] ==0) 
			{
				cout<<"error"<<endl;
				getchar();
			}
		}
		cout << endl;
	}
	//getchar();
	/*cout<<endl<<"edges"<<endl;
	for(int i=0;i<Ins.num_v;i++)
	{
		for(int j=0;j<Ins.num_v;j++)
			if(Ins.edges[i][j])
				cout<<i+1<<" "<< j+1<<endl;		
	}*/
	
#endif
}

//allocate memory for some global variables
void allocate_memory()
{
	Sol_best.ss = new int[Ins.num_v];
	Sol_cur.ss = new int[Ins.num_v];	
	Sol_best_g.ss = new int[Ins.num_v];
	Delta_matrix = new int[Ins.num_v];
	Tabu_list = new int[Ins.num_v];
	int number_neighbors = Ins.num_v*(Ins.num_v - 1) / 2;
	Neighbors = new neighborhood_data[number_neighbors];
	Frequency_arr = new int[Ins.num_v];
	Pop = new solution_data[Pop_size];
	for(int i=0;i<Pop_size;i++)
		Pop[i].ss = new int[Ins.num_v];
	Child.ss = new int[Ins.num_v];
	Child_oppo.ss = new int[Ins.num_v];
}

void free_memory()
{
	delete []Sol_best.ss; Sol_best.ss = NULL; 
	delete []Sol_best_g.ss; Sol_best_g.ss = NULL; 
	delete []Sol_cur.ss; Sol_cur.ss = NULL;
	delete []Delta_matrix; Delta_matrix = NULL;
	delete []Tabu_list; Tabu_list = NULL;
	delete []Neighbors;	Neighbors = NULL;
	delete []Frequency_arr; Frequency_arr = NULL;
	for(int i=0;i<Pop_size;i++)
	{
		delete[] Pop[i].ss; 
		Pop[i].ss = NULL;
	}
	delete []Pop; Pop =NULL;
	delete [] Child.ss; Child.ss=NULL;
	
} 

//generate an initial solution by a greedy manner, based on value density
void greedy_construction(solution_data &sol)
{
	double value_d;
	double value_d_max=-MAXVALUE;
	bool is_compatible;
	for(int i=0;i<Ins.num_v;i++)
		sol.ss[i]=0;
	sol.cost=0;
	sol.weight=0;
	int target_arr[MAXNUM];
	int target_len=0;
	while(1)
	{
		value_d_max = -MAXVALUE;
		target_len=0;
		for(int i=0;i<Ins.num_v;i++)
		{	
			if(!sol.ss[i] && Ins.weight[i] + sol.weight < Ins.capacity) 
			{
				value_d=Ins.profit[i];
				is_compatible=1;
				for(int j=0;j<Ins.adj_len[i];j++)
				{
					int ver=Ins.adj_table[i][j];
					if(sol.ss[ver])
					{
						is_compatible=0;
						break;
					}						
				}
				if(is_compatible)
				{			
					for(int j=0;j<Ins.num_v;j++)
					{						
						if(sol.ss[j])
							value_d+=Ins.quad_profit[i][j];			
					}				
					if(value_d / Ins.weight[i] > value_d_max + PRECISION)
					{
						target_len=0;
						target_arr[target_len++]=i;
						value_d_max=value_d / Ins.weight[i];
					}	
					else if(fabs(value_d / Ins.weight[i] - value_d_max) <= PRECISION && target_len < MAXNUM)
					{
						target_arr[target_len++]=i;
					}		
				
					
				}
			}
			
		}	
		if(target_len >0) 
		{
			int sel=rand()%target_len;
			sel=target_arr[sel];
			sol.ss[sel]=1;
			sol.weight+=Ins.weight[sel];
			//cout<<"sel="<<sel<<endl;
		}
		else	
		{
			break;
		}		
	}
	sol.cost=compute_obj(sol);
}

//help function: random shuffle 
void help_func_random_shuffle(int *rand_arr, int len)
{	
	for(int i = 0; i < len; i++)
	{
		int num = i + rand() % (len - i);		 // È¡Ëæ»úÊý
		int temp = rand_arr[i];
		rand_arr[i] = rand_arr[num];
		rand_arr[num] = temp; 					
	}
}

//generate an initial solution by a random manner
void random_construction(solution_data &sol)
{
	int ver, min_w, is_compatible;
	int *rand_arr = new int[Ins.num_v];
	min_w=Ins.capacity+1;
	for(int i=0;i<Ins.num_v;i++)
	{
		if(Ins.weight[i]<min_w)
			min_w=Ins.weight[i];
	}
	for(int i=0;i<Ins.num_v;i++)
		sol.ss[i]=0;
	sol.cost=0;
	sol.weight=0;	
	
	for(int i=0; i<Ins.num_v; i++)
		rand_arr[i] = i;
	help_func_random_shuffle(rand_arr, Ins.num_v);
	ver=rand_arr[0];
	sol.weight+=Ins.weight[ver];
	sol.ss[ver]=1;
	for(int i=1; i<Ins.num_v; i++)
	{
		is_compatible=1;		
		ver = rand_arr[i];		
		for(int j=0; j < Ins.adj_len[ver]; j++)
		{
			int adj_ver=Ins.adj_table[ver][j];
			if(sol.ss[adj_ver])
			{
				is_compatible=0;
				break;
			}	
		}
		if(is_compatible && sol.weight + Ins.weight[ver] <= Ins.capacity)
		{
			sol.weight+=Ins.weight[ver];
			sol.ss[ver]=1;
			//cout<<"ver="<<ver<<endl;
		}		
	}
	sol.cost=compute_obj(sol);
	delete []rand_arr; rand_arr=NULL;
}


//compute the objective of a solution
int compute_obj(solution_data sol)
{
	int obj=0;
	for(int i=0;i<Ins.num_v;i++)
	{
		if(sol.ss[i])
		{
			obj+=Ins.profit[i];			
			for(int j=i+1;j<Ins.num_v;j++)
			{
				if(sol.ss[j])
				{
					obj+=Ins.quad_profit[i][j];
				}
			}		
		}	
	}
	return obj;
}

//compute the objective of a solution, based on Delta_matrix
int compute_obj2(solution_data sol)
{
	int obj=0;
	int obj_quad=0;
	for(int i=0;i<Ins.num_v;i++)
	{
		if(sol.ss[i])
		{
			obj+=Ins.profit[i];			
			obj_quad+=Delta_matrix[i];
		}	
	}
	obj_quad/=2;
	obj +=obj_quad;
	return obj;
}


bool is_feasible_add_move(solution_data sol, int ver)
{
	bool feasible=true;
	if(sol.weight+Ins.weight[ver]>Ins.capacity)
		feasible=false;
	if(feasible)
	{
		for(int i=0;i<Ins.adj_len[ver];i++)
		{
			int adj_ver=Ins.adj_table[ver][i];
			if(sol.ss[adj_ver])
			{
				feasible=false;
				break;
			}
		}
	}	
	return feasible;
}

bool is_feasible_swap_move(solution_data sol, int v_out, int v_in)
{
	bool feasible = true;
	if(sol.weight + Ins.weight[v_in] - Ins.weight[v_out] > Ins.capacity)
		feasible = false;
	if(feasible)
	{
		for(int i=0; i < Ins.adj_len[v_in]; i++)
		{
			int adj_ver = Ins.adj_table[v_in][i];
			if(sol.ss[adj_ver] && adj_ver != v_out) 
			{
				feasible = false;
				break;
			}
		}
	}
	return feasible;
}

bool is_feasible_two_one_exchange_move(solution_data sol, int v_out1, int v_out2, int v_in)
{
	bool feasible = true;
	if(sol.weight + Ins.weight[v_in] - Ins.weight[v_out1] - Ins.weight[v_out2] > Ins.capacity)
		feasible = false;
	if(feasible)
	{
		for(int i=0; i < Ins.adj_len[v_in]; i++)
		{
			int adj_ver = Ins.adj_table[v_in][i];
			if(sol.ss[adj_ver] && adj_ver != v_out1 && adj_ver != v_out2) 
			{
				feasible = false;
				break;
			}
		}
	}
	return feasible;
}

void build_delta_matrix(solution_data sol)
{
	for(int i=0;i<Ins.num_v;i++)
	{
		Delta_matrix[i]=0;
		for(int j=0;j<Ins.num_v;j++)
		{
			if(sol.ss[j] && i!=j)
				Delta_matrix[i]+=Ins.quad_profit[i][j];
		}
	}	
}

void update_delta_matrix(char move_type, int v_out, int v_out2, int v_in)
{
	switch(move_type)
	{
		case 'a':
			for(int i=0; i< Ins.num_v; i++)
			{
				if(i != v_in)
					Delta_matrix[i] += Ins.quad_profit[i][v_in];	
			}
			break;
		case 'd':
			for(int i = 0; i < Ins.num_v; i++)
			{
				if(i != v_out)
					Delta_matrix[i] -= Ins.quad_profit[i][v_out];	
			}
			break;
		case 's':
			for(int i = 0; i < Ins.num_v; i++)
			{
				if(i != v_out && i != v_in)
				{				
					Delta_matrix[i] -= Ins.quad_profit[i][v_out];	
					Delta_matrix[i] += Ins.quad_profit[i][v_in];		
				}
			}
			Delta_matrix[v_out]+=Ins.quad_profit[v_out][v_in];
			Delta_matrix[v_in] -= Ins.quad_profit[v_in][v_out];	
			break;
		case 'e':
			for(int i = 0; i < Ins.num_v; i++)
			{
				if(i != v_out && i != v_out2 && i != v_in)
				{				
					Delta_matrix[i] -= Ins.quad_profit[i][v_out];	
					Delta_matrix[i] -= Ins.quad_profit[i][v_out2];
					Delta_matrix[i] += Ins.quad_profit[i][v_in];		
				}
			}
			Delta_matrix[v_out] += Ins.quad_profit[v_out][v_in] - Ins.quad_profit[v_out][v_out2];
			Delta_matrix[v_out2] += Ins.quad_profit[v_out2][v_in] - Ins.quad_profit[v_out2][v_out];
			Delta_matrix[v_in] -= (Ins.quad_profit[v_in][v_out] + Ins.quad_profit[v_in][v_out2]); 
			break;
	}
}

void add_move(solution_data &sol, int v_in)
{
	sol.ss[v_in] = 1;
	sol.weight += Ins.weight[v_in];
	update_delta_matrix('a', INFEA, INFEA, v_in);
}

void drop_move(solution_data &sol, int v_out)
{
	sol.ss[v_out] = 0;
	sol.weight -= Ins.weight[v_out];
	update_delta_matrix('d', v_out, INFEA, INFEA);
}

void swap_move(solution_data &sol, int v_out, int v_in)
{
	sol.ss[v_out] =0;
	sol.ss[v_in]=1;
	sol.weight+= Ins.weight[v_in] - Ins.weight[v_out];
	update_delta_matrix('s', v_out, INFEA, v_in);
}

void two_one_exchange_move(solution_data &sol, int v_out1, int v_out2, int v_in)
{
	sol.ss[v_out1] = 0;
	sol.ss[v_out2] = 0;	
	sol.ss[v_in]=1;
	sol.weight+= Ins.weight[v_in] - Ins.weight[v_out1] - Ins.weight[v_out2];
	update_delta_matrix('e', v_out1, v_out2, v_in);
}

//for the shake method
void build_neighbors()
{
	int count = 0;
	//swap neighborhood
	for (int i = 0; i < Ins.num_v; i++)
	{
		for (int j = i + 1; j < Ins.num_v; j++)
		{			
			Neighbors[count].x = i;
			Neighbors[count].y = j;
			count++;
		}
	}
}

int set_shake_length(solution_data sol)
{
	int in_sol=0;
	for(int i=0;i<Ins.num_v;i++)
		if(sol.ss[i])
			in_sol++;
	int shake_length;
	if(Shake_str * in_sol < 1)
		shake_length = 1;
	else
		shake_length = Shake_str * in_sol;
	return shake_length;
}

//random shake
void random_shake(solution_data &sol, int shake_len)
{
	int count = 0;
	int tries = 0;
	int number_neighbors = Ins.num_v * (Ins.num_v - 1) / 2;
	do
	{
		int cur_index = rand() % number_neighbors;			
		int x = Neighbors[cur_index].x;
		int y = Neighbors[cur_index].y;
		if (sol.ss[x] && !sol.ss[y] && is_feasible_swap_move(sol, x, y))
		{
			swap_move(sol, x, y);
			count++;
		}
		else if(!sol.ss[x] && sol.ss[y] && is_feasible_swap_move(sol, y, x))
		{
			swap_move(sol, y, x);
			count++;
		}		
		tries ++;
	} while (count < shake_len && tries < 10000);
	sol.cost = compute_obj2(sol);
}

void help_func_sort(int *a, int *b, int length)
{
    for(int j=0; j<length; j++)
    {
		for(int i=0; i<length-1-j; i++)
        {
			if(a[i] < a[i+1])
            {
                int t=a[i];
                a[i]=a[i+1];
                a[i+1]=t; 
 
                int t1=b[i];
                b[i]=b[i+1];
				b[i+1]=t1;         
			}
		}
	}
}


//shake based on frequency
void frequency_shake(solution_data &sol, int shake_len)
{
	int *frequency_index = new int[Ins.num_v];
	for(int i=0; i< Ins.num_v;i++)
		frequency_index[i] = i;
	//for(int i =0;i<Ins.num_v;i++)
	///	cout<<Frequency_arr[i]<<" ";
	//cout<<"-----------------------------------------"<<endl;
	help_func_sort(Frequency_arr, frequency_index, Ins.num_v);
	//for(int i =0;i<Ins.num_v;i++)
	//	cout<<Frequency_arr[i]<<" ";
	//cout<<"-----------------------------------------"<<endl;
	//for(int i=0; i< Ins.num_v;i++)
	//	cout<<frequency_index[i]<<" ";
		
	int count = 0;	
	int start1 =0;
	int start2 = Ins.num_v-1;	

	for(int i=start1;i<Ins.num_v;i++)
	{
		int ver_in = frequency_index[i];
		if(sol.ss[ver_in])
		{
			for(int j=start2; j >=0; j--)
			{					
				int ver_out = frequency_index[j];
				//cout<<"i="<<i<<", j="<<j<<", ver_in="<<ver_in<<", ver_out="<<ver_out<<endl;
				if (sol.ss[ver_in] && !sol.ss[ver_out] && is_feasible_swap_move(sol, ver_in, ver_out))
				{
					swap_move(sol, ver_in, ver_out);
					//cout<<"i="<<i<<", j= "<<j<<", count="<<count<<endl;
					//getchar();
					count++;
					start1 = i;
					start2 = j;
				}	
			}
		}		
	}
	cout<<"count="<<count<<endl;
	//	getchar();
	sol.cost = compute_obj2(sol);	
	delete []frequency_index; frequency_index = NULL;
}

bool is_same_solution(solution_data sol, int index)
{
	bool is_same = true;
	if(sol.cost != Pop[index].cost || sol.weight !=Pop[index].weight)
		is_same = false;
	if(is_same)
	{
		for(int i=0;i<Ins.num_v;i++)
		{
			if(sol.ss[i]!=Pop[index].ss[i])
			{
				is_same = false;
				break;
			}	
		}			
	}	
	return is_same;
}

void copy_solution(solution_data src, solution_data &des)
{
	des.cost = src.cost;
	des.weight = src.weight;
	for (int i = 0; i < Ins.num_v; i++)
		des.ss[i] = src.ss[i];
}

void out_results_best_sol(char *out_filename, char instance_name[], solution_data sol)
{
	FILE *fp;
	char buff[255];
	sprintf(buff, "%s", out_filename);
	fp = fopen(buff, "a+");
	int len = 0;
	for (int i = 0; i < Ins.num_v; i++)
		if (sol.ss[i])
			len++;
	fprintf(fp, "%s,cost=%d %d\n", instance_name, sol.cost, len);
	for (int i = 0; i < Ins.num_v; i++)
		if(sol.ss[i])
			fprintf(fp, "%d ", i+1);
	fprintf(fp, "\n");
	fclose(fp);
}

void out_results_stat(int best, double ave, int worst, double avg_time, int hit, char *stat_filename, char instance_name[])
{
	FILE *fp;
	char buff[255];
	sprintf(buff, "%s", stat_filename);
	fp = fopen(buff, "a+");
	fprintf(fp, "%s %d %f %d %d %f\n", instance_name, best, ave, worst, hit, avg_time);
	fclose(fp);
}

//verify the obtained solution
void verify_solution(solution_data sol)
{
	int weight=0;
	bool is_compatible=1;
	for(int i=0;i<Ins.num_v;i++)
	{
		if(sol.ss[i]) 
		{	
			weight+=Ins.weight[i];					
			for(int j=0;j<Ins.adj_len[i];j++)
			{
				int ver=Ins.adj_table[i][j];
				if(sol.ss[ver])
				{
					is_compatible=0;
					break;
				}		
			}
			if(!is_compatible)
				break;
		}
	}
	if(weight != sol.weight)
	{
		cout<<"in verify_solution method, an error of weight is checked, !!!!!!!!!!!!!!";
		cout<<"weight="<<weight<<", sol.weight="<<sol.weight<<endl;
		getchar();
	}
	if(weight > Ins.capacity)
	{
		cout<<"in verify_solution method, an error of capacity is checked, !!!!!!!!!!!!!!";
		cout<<"weight="<<weight<<", capacity="<< Ins.capacity <<endl;
		getchar();
	}
	if(!is_compatible)
	{
		cout<<"in verify_solution method, an error of compatible is checked, !!!!!!!!!!!!!!";
		getchar();
	}		
	int obj = compute_obj(sol);
	if(obj != sol.cost)
	{
		cout<<"in verify_solution method, an error of objective is checked, !!!!!!!!!!!!!!";
		cout<<"obj ="<< obj <<", sol.cost ="<< sol.cost <<endl;
		getchar();
	}		
}





























