#pragma once
#ifndef GALogic_h
#define GALogic_h

//-----------------------------------------------------------------------------------------------
//
//  Code to illustrate the use of a genetic algorithm by Mat Buckland aka fup
//
//-----------------------------------------------------------------------------------------------
#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>



using namespace std;

#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             0.001
#define POP_SIZE                  100           //must be an even number
#define CHROMO_LENGTH             300
#define GENE_LENGTH               4    //4 bites is 1 gene
#define MAX_ALLOWABLE_GENERATIONS   400

//returns a float between 0 & 1
#define RANDOM_NUM      ((float)rand()/RAND_MAX)

//----------------------------------------------------------------------------------------
//
//  define a data structure which will define a chromosome
//
//----------------------------------------------------------------------------------------
struct chromo_typ
{
	//the binary bit string is held in a std::string
	string    bits;

	float     fitness;

	chromo_typ() : bits(""), fitness(0.0f) {};
	chromo_typ(string bts, float ftns) : bits(bts), fitness(ftns) {}

};


/////////////////////////////////prototypes/////////////////////////////////////////////////////

void    PrintGeneSymbol(int val);
string  GetRandomBits(int length);
int     BinToDec(string bits);
float   AssignFitness(string bits, int target_value);
void    PrintChromo(string bits);
void    PrintGeneSymbol(int val);
int     ParseBits(string bits, int* buffer);
string  Roulette(int total_fitness, vector<chromo_typ> Population);
void    Mutate(string &bits);
void    Crossover(string &offspring1, string &offspring2);
bool	operator <(const chromo_typ& chromo1, const chromo_typ& chromo2);
bool	operator >(const chromo_typ& chromo1, const chromo_typ& chromo2);

#endif 
