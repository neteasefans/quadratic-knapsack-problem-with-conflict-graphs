#include"func_state.h"
#include"global_variables.h"

//#include"dirent.h" 	// this file is required in Winndows system

#include <sys/types.h>
#include<dirent.h>

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		cout << "HSM_QKPCG usage: instance_dir output_sol_file out_stat_file" << endl;;
		exit(10);
	}
	string path = argv[1];
	string out_sol_file = argv[2];
	string out_stat_file = argv[3];
	string instance_name = path;

	
	//char *instance_name = argv[1];	
	//time limit , data1: 50s; data2: 100s; data3: 200s

	
		
	DIR * dir;
    struct dirent * ptr;
    dir = opendir(path.c_str()); 
	srand(unsigned(time(NULL)));
	
	Time_limit = 20;
	Runs = 10;		

	Tabu_tenure = 20;
	Non_imp_iters = 10000;
	Pop_size = 10;
	Shake_str = 0.2;			//unused
		
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
			string name_s = name;
			instance_name = path + "/" + name_s;
			read_instance(instance_name.c_str());		
		
			allocate_memory();
			build_neighbors();
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
				out_results_best_sol(out_sol_file.c_str(), name, Sol_best);
				//out_results_one_run(out_sol_name, Instance_name, Best_k, Run_time);
			}		
				
		ff_avg = ff_avg / Runs;
		avg_time /= Runs;
		verify_solution(Sol_best_g);			
		out_results_stat(ff_best, ff_avg, ff_worst, avg_time, hit, out_stat_file.c_str(), instance_name.c_str());
		free_memory();
		}
	}
	closedir(dir);
	//getchar();
}
