#ifndef FUNC_STATE_H
#define	FUNC_STATE_H
#include"global_variables.h"
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<fstream>
#include<string.h>
#include<math.h>

using namespace std;

//read instance file
void read_instance(char *instance_name);	

//allocate memory for some global variables
void allocate_memory();

void free_memory();

//generate an initial solution by a greedy manner
void greedy_construction(solution_data &sol); 

//help function: random shuffle 
void help_func_random_shuffle(int *rand_arr, int len);

//generate an initial solution by a random manner
void random_construction(solution_data &sol); 

//compute the objective of a solution, original manner
int compute_obj(solution_data sol);

//compute the objective of a solution, based on Delta_matrix
int compute_obj2(solution_data sol);

//verify whether the candidate add move is feasible or not
bool is_feasible_add_move(solution_data sol, int ver);

//verify whether the candidate swap move is feasible or not
bool is_feasible_swap_move(solution_data sol, int v_out, int v_in);

//verify whether the candidate 2-1 exchange move is feasible or not£¬ unused 
bool is_feasible_two_one_exchange_move(solution_data sol, int v_out1, int v_out2, int v_in);

//build the Delta_matrix for fast incremental evaluation 
void build_delta_matrix(solution_data sol);

//update the Delta_matrix for fast incremental evaluation 
void update_delta_matrix(char move_type, int v_out, int v_out2, int v_in);

//execute the add move
void add_move(solution_data &sol, int v_in);

//execute the drop move
void drop_move(solution_data &sol, int v_out);

//execute the swap move
void swap_move(solution_data &sol, int v_out, int v_in);

//execute the 2-1 exchange move, unused
void two_one_exchange_move(solution_data &sol, int v_out1, int v_out2, int v_in);

//verify the obtained solution
void verify_solution(solution_data sol);

//multi neighborhood tabu search
void multi_neighbor_tabu_search(solution_data &sol);

//multi neighborhood tabu search, containing 2-1 exchange move, unused
void multi_neighbor_tabu_search2(solution_data &sol);

//for shake method
void build_neighbors();

int set_shake_length(solution_data sol);

//random shake
void random_shake(solution_data &sol, int shake_len);

//shake based on frequency
void frequency_shake(solution_data &sol, int shake_len);

//for evolutionary search
void initial_population();

//crossover operator based on uniform
void uniform_crossover(solution_data &sol_child);

//crossover operator based on backbone
void backbone_crossover(solution_data &sol_child);

//population updating strategy: replaces the worst solution in the population
void update_population();

//evolutionay search method
void evolutionary_search();

//copy a solution
void copy_solution(solution_data src, solution_data &des);

//verify whether the solution is the same as any solution in the population
bool is_same_solution(solution_data sol, int index);

//output the found best solution of each run
void out_results_best_sol(char *out_filename, char instance_name[], solution_data sol);

//output the found best results of each run
void out_results_stat(int best, double ave, int worst, double avg_time, int hit, char *stat_filename, char instance_name[]);

#endif
