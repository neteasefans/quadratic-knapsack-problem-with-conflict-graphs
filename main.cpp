#include"func_state.h"
#include"global_variables.h"
#include"dirent.h"

#include <sys/types.h>


int main(int argc, char *argv[])
{
	char out_sol_file[] = "F://knapsack problem//QKPCG(quadratic knapsack problem with conflict graphs)//HESA_QKPCG//output_dir_test//out_sol_HESA_test_ana.txt";
	char out_stat_file[] = "F://knapsack problem//QKPCG(quadratic knapsack problem with conflict graphs)//HESA_QKPCG//output_dir_test//out_stat_HESA_test_ana.txt";

	//if (argc < 3)
	//{
	//	cout << "TS.exe usage: instance_file seed";
	//	exit(10);
	//}
	
	//char *instance_name = argv[1];	
	//time limit , data1: 50s; data2: 100s; data3: 200s
	char path[]="F://knapsack problem//QKPCG(quadratic knapsack problem with conflict graphs)//HESA_QKPCG//data_test//";
	char instance_name[256] = "F://knapsack problem//QKPCG(quadratic knapsack problem with conflict graphs)//HESA_QKPCG//data_test//";
	
	int path_len = strlen(path);
	int seed;
		
	DIR * dir;
    struct dirent * ptr;
    int i;
    dir = opendir(path); 
	srand(unsigned(time(NULL)));
	
	Time_limit = 10;
	Tabu_tenure = 20;
	Shake_str = 0.2;
	Non_imp_iters = 10000;
	Pop_size = 10;
	Runs = 100;		
		
	 while((ptr = readdir(dir)) != NULL)
    {
    	int ff_best = -MAXVALUE;
		int ff_worst = MAXVALUE;
		int hit = 0;
		double ff_avg = 0;
		double avg_time = 0;
    	if(strcmp(".", ptr->d_name) && strcmp("..", ptr->d_name))
		{			
			char *name = ptr->d_name;						
			for(int j=0; j< strlen(name); j++) 
				instance_name[path_len + j] =  name[j];
			instance_name[path_len + strlen(name)] ='\0';		
			read_instance(instance_name);		
		
			allocate_memory();
			build_neighbors();
			Time_limit = Ins.num_v / 2;
			for (int i = 0; i < Runs; i++)
			{	
				Start_time = clock();
				//greedy_construction(Sol_cur);
				//random_construction(Sol_cur);
				//copy_solution(Sol_cur, Sol_best);
				//multi_neighbor_tabu_search(Sol_cur);
				
				evolutionary_search();
		
				if (Sol_best.cost > ff_best)
				{
					hit = 1;
					ff_best = Sol_best.cost;
					copy_solution(Sol_best, Sol_best_g);
				}
				else if (Sol_best.cost == ff_best)
					hit++;
				if (Sol_best.cost < ff_worst)
					ff_worst = Sol_best.cost;
				ff_avg += Sol_best.cost;
				avg_time += Run_time;		
				out_results_best_sol(out_sol_file, name, Sol_best);
				//out_results_one_run(out_sol_name, Instance_name, Best_k, Run_time);
			}		
				
		ff_avg = ff_avg / Runs;
		avg_time /= Runs;
		verify_solution(Sol_best_g);			
		out_results_stat(ff_best, ff_avg, ff_worst, avg_time, hit, out_stat_file, instance_name);
		free_memory();
		}
	}
	closedir(dir);
	//getchar();
}
