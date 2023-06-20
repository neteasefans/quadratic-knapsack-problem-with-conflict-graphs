#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

typedef struct instance_data
{
	int num_v;							//number of items 
	int num_edges;						//number of edges
	int capacity;						//capacity constraint
	int *weight;						//weight of each item
	int *profit; 						//profit of each item
	int **quad_profit;					//quadratic profit between each pair of items
//	int **edges;						//edge matrix, edges[i][j] == 1 indicates that items i and j are incompatible, 0 otherwise , unused
	int **adj_table;					//adjacent table, adj_table[i][j] is the j-th vertex  incompatible to vertex i
	int *adj_len;						//adj_len[i] is the number of vertices incompatible to vertex i
}instance_data;

typedef struct solution_data
{
	int cost;							//the objective of solution
	int weight;							//the total weight of the solution
	int	*ss;							//representation of the solution, ss[i] = 1 indicates that vertex i is selected, 0 otherwise
}solution_data;

typedef struct neighborhood{	
	int  x;
	int  y;
}neighborhood_data;

//global variables are all started with Capital letter
extern instance_data Ins;				//instance data
extern solution_data Sol_best_g;		//best solution for all runs;
extern solution_data Sol_best;			//best solution found for each run
extern solution_data Sol_cur;			//current solution
extern int *Delta_matrix;				//delta_matrix for fast incremental evaluation of candidate move

extern neighborhood_data *Neighbors;
extern double Shake_str;				//shake strength
extern int Shake_non_imp;				
extern int Non_imp_iters;				//maximum consecutive iterations of non-improve of tabu search
extern int Runs;	
extern int *Tabu_list;
extern int Tabu_tenure;					
extern int *Frequency_arr;				 

//for evolutionary search
extern int Pop_size;					//population size
extern solution_data *Pop;				//population
extern solution_data Child;				//child solution
extern solution_data Child_oppo;		//child solution

extern double Start_time, Run_time, Time_limit;

#define DEBUG_READ_INSTANCE
#define PRECISION 1.0e-8
#define MAXVALUE 99999999
#define MAXNUM 100
#define INFEA -1

#endif
