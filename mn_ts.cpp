#include"global_variables.h"
#include"func_state.h"

//multi neighborhood tabu search
void multi_neighbor_tabu_search(solution_data &sol)
{
//	int add_delta,drop_delta,swap_delta;
//	int tabu_add_delta,tabu_drop_delta,tabu_swap_delta;
	int delta, tabu_delta;
	int add_delta_max,drop_delta_max,swap_delta_max;
	int tabu_add_delta_max,tabu_drop_delta_max,tabu_swap_delta_max;
	int add_arr[MAXNUM],drop_arr[MAXNUM],swap_arr1[MAXNUM], swap_arr2[MAXNUM];
	int tabu_add_arr[MAXNUM],tabu_drop_arr[MAXNUM],tabu_swap_arr1[MAXNUM], tabu_swap_arr2[MAXNUM];
	int add_len, drop_len, swap_len;
	int tabu_add_len, tabu_drop_len, tabu_swap_len;
	int iter=0;
	int non_improve = 0;
	int count_add=0, count_drop=0, count_swap=0;
	solution_data local_best;
	local_best.ss = new int[Ins.num_v];
	copy_solution(sol, local_best);
	for(int i=0; i< Ins.num_v; i++)
		Tabu_list[i] = 0;
	build_delta_matrix(sol);	
	for(int i=0;i<Ins.num_v; i++)
		Frequency_arr[i]=0;	
	//while((1.0*clock()-Start_time)/CLOCKS_PER_SEC < Time_limit)
	while(non_improve < Non_imp_iters)
	{		
		//for add move, complexity: O(|E|), |E| is the set of incompatible items 
		add_delta_max=-MAXVALUE;
		tabu_add_delta_max=-MAXVALUE;
		add_len=0;
		tabu_add_len=0;
		for(int i=0;i<Ins.num_v;i++)
		{
			if(!sol.ss[i] && is_feasible_add_move(sol, i))  
			{
				delta=Ins.profit[i]+Delta_matrix[i];
				if(Tabu_list[i]<= iter)
				{
					if(delta>add_delta_max)
					{
						add_delta_max=delta;
						add_len=0;
						add_arr[add_len++]=i;					
					}
					else if(delta==add_delta_max && add_len < MAXNUM)
						add_arr[add_len++]=i;
				}
				else
				{
					if(delta>tabu_add_delta_max)
					{
						tabu_add_delta_max=delta;
						tabu_add_len=0;
						tabu_add_arr[tabu_add_len++]=i;
					}
					else if(delta==tabu_add_delta_max && tabu_add_len <MAXNUM)
						tabu_add_arr[tabu_add_len++]=i;
				}	
			}
		}
		
		//for drop move
		drop_delta_max=-MAXVALUE;
		tabu_drop_delta_max=-MAXVALUE;
		drop_len=0;
		tabu_drop_len=0;
		for(int i=0;i<Ins.num_v;i++)
		{
			if(sol.ss[i])  
			{
				delta=-(Ins.profit[i]+Delta_matrix[i]);
				//if(Tabu_list[i]<=iter)
				{
					if(delta>drop_delta_max)
					{
						drop_delta_max=delta;
						drop_len=0;
						drop_arr[drop_len++]=i;					
					}
					else if(delta==drop_delta_max && drop_len < MAXNUM)
						drop_arr[drop_len++]=i;
				}
				/*else
				{
					if(delta>tabu_drop_delta_max)
					{
						tabu_drop_delta_max=delta;
						tabu_drop_len=0;
						tabu_drop_arr[tabu_drop_len++]=i;
					}
					else if(delta==tabu_drop_delta_max && tabu_drop_len <MAXNUM)
						tabu_drop_arr[tabu_drop_len++]=i;
				}*/
			}
		}		
		
		//for swap move
		swap_delta_max=-MAXVALUE;
		tabu_swap_delta_max=-MAXVALUE;
		swap_len=0;
		tabu_swap_len=0;
		for(int i=0;i<Ins.num_v;i++)
		{
			if(sol.ss[i])
			{
				for(int j=0;j<Ins.num_v;j++)
				{
					if(!sol.ss[j] && is_feasible_swap_move(sol, i, j))
					{
						delta=Ins.profit[j]-Ins.profit[i]+Delta_matrix[j]-Delta_matrix[i]-Ins.quad_profit[i][j];
						//if(Tabu_list[i] <= iter && Tabu_list[j] <= iter)
						if(Tabu_list[j] <= iter)
						{
							if(delta>swap_delta_max)
							{
								swap_delta_max=delta;
								swap_len=0;
								swap_arr1[swap_len]=i;					
								swap_arr2[swap_len]=j;
								swap_len++;
							}
							else if(delta==swap_delta_max && swap_len < MAXNUM)
							{		
								swap_arr1[swap_len]=i;									
								swap_arr2[swap_len]=j;
								swap_len++;
							}	
						}
						else
						{
							if(delta>tabu_swap_delta_max)
							{
								tabu_swap_delta_max=delta;
								tabu_swap_len=0;
								tabu_swap_arr1[tabu_swap_len]=i;
								tabu_swap_arr2[tabu_swap_len]=j;
								tabu_swap_len++;								
								
							}
							else if(delta==tabu_swap_delta_max && tabu_swap_len <MAXNUM)
							{
								tabu_swap_arr1[tabu_swap_len]=i;
								tabu_swap_arr2[tabu_swap_len]=j;
								tabu_swap_len++;
							}
						}	
					}
				}
			}
		}		
		
		//select the best move from add, drop, swap neighborhoods		
		int move_type = INFEA;
		int delta_maximum = -MAXVALUE;
		//cout<<"add_delta_max="<<add_delta_max<<", tabu_add_delta_max="<<tabu_add_delta_max<<", drop_delta_max="<<drop_delta_max<<", tabu_drop_delta_max"<<tabu_drop_delta_max 
		//	<<", swap_delta_max="<<swap_delta_max<<", tabu_swap_delta_max="<<tabu_swap_delta_max<<endl;
		//cout<<" add_len="<<add_len<<", tabu_add_len="<<tabu_add_len<<", drop_len="<<drop_len<<", tabu_dorp_len="<<tabu_drop_len<<", swap_len=" <<swap_len<<", tabu_swap_len="<<tabu_swap_len<<endl;		
		if (add_len > 0 && add_delta_max > delta_maximum)
		{
			move_type = 1;
			delta_maximum = add_delta_max;
		}
		if ((tabu_add_len > 0 && tabu_add_delta_max > delta_maximum && sol.cost + tabu_add_delta_max > local_best.cost))
		{
			move_type = 2;
			delta_maximum = tabu_add_delta_max;
		}
		if (drop_len > 0 && drop_delta_max > delta_maximum)
		{
			move_type = 3;
			delta_maximum = drop_delta_max;
		}
		if ((tabu_drop_len > 0 && tabu_drop_delta_max > delta_maximum && sol.cost + tabu_drop_delta_max > local_best.cost))
		{
			move_type = 4;
			delta_maximum = tabu_drop_delta_max;
		}
		if (swap_len > 0 && swap_delta_max > delta_maximum)
		{
			move_type = 5;
			delta_maximum = swap_delta_max;
		}
		if ((tabu_swap_len > 0 && tabu_swap_delta_max > delta_maximum && sol.cost + tabu_swap_delta_max > local_best.cost)) 
		{
			move_type = 6;
			delta_maximum = tabu_swap_delta_max;
		}		
		int item1 = INFEA, item2 = INFEA, item = INFEA;
		int rx = INFEA;
		switch(move_type)
		{
			case 1:
				rx = rand() % add_len;
				item = add_arr[rx];
				break;
			case 2:
				rx = rand() % tabu_add_len;
				item = tabu_add_arr[rx];
				break;
			case 3:
				rx = rand() % drop_len;
				item = drop_arr[rx];
				break;
			case 4:
				rx = rand() % tabu_drop_len;
				item = tabu_drop_arr[rx];
				break;
			case 5:
				rx = rand() % swap_len;
				item1 = swap_arr1[rx];
				item2 = swap_arr2[rx];
				break;
			case 6:
				rx = rand() % tabu_swap_len;
				item1 = tabu_swap_arr1[rx];
				item2 = tabu_swap_arr2[rx];
				break;
		}
	
		if (move_type == 1 || move_type == 2)
		{
			add_move(sol, item);			
			//Tabu_list[item] = Tabu_tenure + iter;			
			Frequency_arr[item]++;
		}
		else if (move_type == 3 || move_type == 4)
		{		
			drop_move(sol, item);
			Tabu_list[item] = Tabu_tenure + iter;
			Frequency_arr[item]++;
		}
		else if (move_type == 5 || move_type == 6)
		{		
			swap_move(sol, item1, item2);
			Tabu_list[item1] = Tabu_tenure + iter;
			//Tabu_list[item2] = Tabu_tenure + iter;		
			Frequency_arr[item1]++;
			Frequency_arr[item2]++;
		}
		if (move_type != INFEA)
		{
			sol.cost += delta_maximum;
			if (sol.cost > local_best.cost)
			{
				non_improve = 0;				
				copy_solution(sol, local_best);
			}
			else
				non_improve++;			
			if (sol.cost > Sol_best.cost)
			{
				copy_solution(sol, Sol_best);
				Run_time = (1.0 * clock() - Start_time) / CLOCKS_PER_SEC;
			}	
		}		
		iter++;		
		//verify_solution(sol);		
	}	
	copy_solution(local_best, sol);
	delete[]local_best.ss ; local_best.ss  =NULL;
}


































