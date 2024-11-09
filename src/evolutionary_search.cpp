#include"func_state.h"



void initial_population()
{
	int size=0;
	while(size < Pop_size)
	{
		random_construction(Pop[size]);
		if (Pop[size].cost > Sol_best.cost)
		{
			copy_solution(Pop[size], Sol_best);
			Run_time = (1.0 * clock() - Start_time) / CLOCKS_PER_SEC;
		}	
		multi_neighbor_tabu_search(Pop[size]);
		bool is_same = false;		
		for(int i=0; i < size;i++)
		{		
			is_same = is_same_solution(Pop[size], i); 
			if(is_same)			
				break;			
		}		
		if(is_same)
		{
			int shake_length = set_shake_length(Pop[size]);
			int tries =0;
			while(1)
			{
				build_delta_matrix(Pop[size]);
				random_shake(Pop[size], shake_length);				
				shake_length = set_shake_length(Pop[size]);
				tries++;
				int count_dif =0;
				for(int i=0;i<size;i++)
					if(!is_same_solution(Pop[size], i))
					   count_dif++;
				if(count_dif==size)
					break;				
				if(tries > 100) 
					break;
			}	
			cout<<"shaked, size="<<size<<", Pop[size].cost=" << Pop[size].cost<<endl;
			//getchar();	
			size++;	
		}
		else
		{
			cout<<"no shake, size="<<size<<", Pop[size].cost=" << Pop[size].cost<<endl;
			//getchar();	
			size++;
		}    				
	}
}

//crossover operator based on uniform
void uniform_crossover(solution_data &sol_child)
{
	int par1=rand()%Pop_size;
	int par2=rand()%Pop_size;
	int *rand_arr = new int[Ins.num_v];
	while(par2==par1)
		par2=rand()%Pop_size;
	for(int i=0;i<Ins.num_v;i++)
		sol_child.ss[i]=0;
	sol_child.weight=0;
	sol_child.cost=0;
	for(int i=0; i<Ins.num_v; i++)
		rand_arr[i] = i;
	help_func_random_shuffle(rand_arr, Ins.num_v);	
	for(int i=0;i<Ins.num_v;i++)
	{
		int rx = rand()%2;
		int ver = rand_arr[i];
		if(rx ==0 && Pop[par1].ss[ver] && is_feasible_add_move(sol_child, ver))
		{
			sol_child.ss[ver] = Pop[par1].ss[ver];
			sol_child.weight+=Ins.weight[ver];			
		}
		else if(rx == 1 && Pop[par2].ss[ver] && is_feasible_add_move(sol_child, ver))
		{
			sol_child.ss[ver] = Pop[par2].ss[ver];
			sol_child.weight+=Ins.weight[ver];
		}
	}
	sol_child.cost = compute_obj(sol_child);
	delete []rand_arr; rand_arr=NULL;
}

//crossover operator based on backbone
void backbone_crossover(solution_data &sol_child)
{
	int par1=rand()%Pop_size;
	int par2=rand()%Pop_size;
	int *rand_arr = new int[Ins.num_v];
	while(par2==par1)
		par2=rand()%Pop_size;
	for(int i=0;i<Ins.num_v;i++)
		sol_child.ss[i]=0;
	sol_child.weight=0;
	sol_child.cost=0;
	for(int i=0;i<Ins.num_v;i++)
	{
		if(Pop[par1].ss[i] && Pop[par2].ss[i])
		{
			sol_child.ss[i] = Pop[par1].ss[i];
			sol_child.weight+=Ins.weight[i];
		}
	}
	for(int i=0; i<Ins.num_v; i++)
		rand_arr[i] = i;
	help_func_random_shuffle(rand_arr, Ins.num_v);
	for(int i=0;i<Ins.num_v;i++)
	{
		int ver = rand_arr[i];
		if(sol_child.ss[ver]==0 && is_feasible_add_move(sol_child, ver))
		{
			sol_child.ss[ver] = 1;
			sol_child.weight+=Ins.weight[ver];
		}  
	}
	sol_child.cost = compute_obj(sol_child);	
	delete []rand_arr; rand_arr=NULL;
} 

/*generate opposite solution*/
void generate_opposition_solution(const solution_data &sol_child, solution_data &sol_oppo)
{
	int *rand_node = new int[Ins.num_v];
	int *flag_node = new int[Ins.num_v];
	int terminal_con = false;
	int node, fea_flag;
	for (int i = 0; i < Ins.num_v; i++)
		rand_node[i] = i;
	help_func_random_shuffle(rand_node, Ins.num_v);
	memset(flag_node, 0, sizeof(int)*Ins.num_v);
	memset(sol_oppo.ss, 0, sizeof(int)*Ins.num_v);
	sol_oppo.weight = 0;
	while (!terminal_con)
	{
		terminal_con = true;
		fea_flag = false;
		for (int i = 0; i < Ins.num_v; i++)
		{
			node = rand_node[i];
			if (!sol_child.ss[node] && !flag_node[node] && is_feasible_add_move(sol_oppo, node))
			{
				fea_flag = true;
				break;
			}
		}
		if (fea_flag)
		{
			terminal_con = false;
			sol_oppo.ss[node] = 1;
			sol_oppo.weight += Ins.weight[node];
			flag_node[node] = 1;
		}
	}
	sol_oppo.cost = compute_obj(sol_oppo);
	delete[]rand_node; rand_node = NULL;
	delete[]flag_node; flag_node = NULL;
}

//population updating strategy: replaces the worst solution in the population
void update_population(solution_data sol_child)
{
	int worst_ind = -1;
	int worst_cost = MAXVALUE;
	for(int i=0;i<Pop_size;i++)
	{
		if(Pop[i].cost<worst_cost)
		{
			worst_cost = Pop[i].cost;
			worst_ind = i;
		}
	}	
	if(sol_child.cost > worst_cost)
	{
		bool is_same = false;
		for(int i=0;i<Pop_size;i++)
		{
			is_same = is_same_solution(sol_child, i);
			if(is_same)
				break;
		}
		if(!is_same)		
			copy_solution(sol_child, Pop[worst_ind]);		
	}
} 

//compute the similarity of the population
double compute_pop_sim()
{
	int *sol_len = new int[Pop_size];
	for(int i=0;i<Pop_size; i++)
		sol_len[i]=0; 
	for(int i=0; i<Pop_size; i++)
	{
		for(int j = i+1; j<Pop_size;j++)
		{
			if(is_same_solution(Pop[i], j))
			{
				cout<<"an error found, i = " << i <<", j=" << j<< ", are the same solutions" << endl;
				//getchar();
			}			
		}
	}	
	double sim=0;
	for(int i=0;i<Pop_size;i++)
		for(int j=0;j<Ins.num_v;j++)
			if(Pop[i].ss[j])
				sol_len[i]++;
	//for(int i=0;i<Pop_size;i++)
	//	cout<<sol_len[i]<<" ";	
	//cout<<endl;
	/*for(int i=0;i<Pop_size;i++)
	{
		for(int j=0;j<Ins.num_v;j++)
			if(Pop[i].ss[j])
				cout<<j<<" ";
		cout<<endl;
	}*/
	for(int i=0; i < Pop_size; i++)
	{
		for(int j=i+1;j<Pop_size;j++)
		{
			int len=0;
			for(int k=0;k<Ins.num_v;k++)
				if(Pop[i].ss[k] && Pop[j].ss[k])	
					len++;
			if(sol_len[i] > sol_len[j])
				sim += 1.0*len / sol_len[i];
			else
				sim += 1.0*len / sol_len[j];
			//cout<<"i=" <<i <<", j="<< j << ", len="<<len<<endl;
		}
	}
	sim = sim / (Pop_size * (Pop_size-1) /2);
	delete []sol_len; sol_len = NULL;
	return sim;
}

void evolutionary_search()
{
	int genes = 0;	
	Sol_best.cost = -MAXVALUE;
	initial_population();
	for (int i = 0; i < Pop_size; i++)
		cout << Pop[i].cost << " ";
	while((1.0*clock()-Start_time)/CLOCKS_PER_SEC < Time_limit)
	{
		uniform_crossover(Child);	
		//backbone_crossover(Child);
		//generate_opposition_solution(Child, Child_oppo);
		cout << "after crossover" << ", Child.cost=" << Child.cost << ", Child_oppo.cost=" << Child_oppo.cost << endl;;
		multi_neighbor_tabu_search(Child);
		//cout<<"after local search" <<", Child.cost="<<Child.cost<<endl;	
		update_population(Child); 

		//multi_neighbor_tabu_search(Child_oppo);
		//update_population(Child_oppo);

		double pop_sim = -1;
		//double pop_sim = compute_pop_sim();
		cout<<"genes="<<genes++ <<", Sol_best.cost=" << Sol_best.cost<< ", pop_sim="<< pop_sim << ", time=" << (1.0*clock()-Start_time)/CLOCKS_PER_SEC <<", run_time=" << Run_time<< endl;
		
		cout<<endl;
	}
}





































