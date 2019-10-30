#include "GALogic.h"
using namespace std;

//---------------------------------GetRandomBits-----------------------------------------
//
//  This function returns a string of random 1s and 0s of the desired length.
//
//-----------------------------------------------------------------------------------------
string  GetRandomBits(int length)
{
	string bits;

	for (int i = 0; i < length; i++)
	{
		if (RANDOM_NUM > 0.5f)

			bits += "1";

		else

			bits += "0";
	}

	return bits;
}

//---------------------------------BinToDec-----------------------------------------
//
//  converts a binary string into a decimal integer
//
//-----------------------------------------------------------------------------------
int BinToDec(string bits)
{
	int val = 0;
	int value_to_add = 1;

	for (unsigned long i = bits.length(); i > 0; i--)
	{


		if (bits.at(i - 1) == '1')

			val += value_to_add;

		value_to_add *= 2;

	}//next bit

	return val;
}


//---------------------------------ParseBits------------------------------------------
//
// Given a chromosome this function will step through the genes one at a time and insert 
// the decimal values of each gene (which follow the operator -> number -> operator rule)
// into a buffer. Returns the number of elements in the buffer.
//------------------------------------------------------------------------------------
int ParseBits(string bits, int* buffer)
{

	//counter for buffer position
	int cBuff = 0;

	// step through bits a gene at a time until end and store decimal values
	// of valid operators and numbers. Don't forget we are looking for operator - 
	// number - operator - number and so on... We ignore the unused genes 1111
	// and 1110

	//flag to determine if we are looking for an operator or a number
	bool bOperator = true;

	//storage for decimal value of currently tested gene
	int this_gene = 0;

	for (int i = 0; i < CHROMO_LENGTH; i += GENE_LENGTH)
	{
		//convert the current gene to decimal
		this_gene = BinToDec(bits.substr(i, GENE_LENGTH));

		//find a gene which represents an operator
		if (bOperator)
		{
			if ((this_gene < 10) || (this_gene > 13))

				continue;

			else
			{
				bOperator = false;
				buffer[cBuff++] = this_gene;
				continue;
			}
		}

		//find a gene which represents a number
		else
		{
			if (this_gene > 9)

				continue;

			else
			{
				bOperator = true;
				buffer[cBuff++] = this_gene;
				continue;
			}
		}

	}//next gene

	 //  now we have to run through buffer to see if a possible divide by zero
	 //  is included and delete it. (ie a '/' followed by a '0'). We take an easy
	 //  way out here and just change the '/' to a '+'. This will not effect the 
	 //  evolution of the solution
	for (int i = 0; i < cBuff; i++)
	{
		if ((buffer[i] == 13) && (buffer[i + 1] == 0))

			buffer[i] = 10;
	}

	return cBuff;   //find how many useful genes do we have
}

//---------------------------------AssignFitness--------------------------------------
//
//  given a string of bits and a target value this function will calculate its  
//  representation and return a fitness score accordingly
//------------------------------------------------------------------------------------
float AssignFitness(string bits, int target_value)
{

	//holds decimal values of gene sequence
	int buffer[(int)(CHROMO_LENGTH / GENE_LENGTH)];

	int num_elements = ParseBits(bits, buffer);

	// ok, we have a buffer filled with valid values of: operator - number - operator - number..
	// now we calculate what this represents.
	float result = 0.0f;

	for (int i = 0; i < num_elements - 1; i += 2)
	{
		switch (buffer[i])
		{
		case 10:

			result += buffer[i + 1];
			break;

		case 11:

			result -= buffer[i + 1];
			break;

		case 12:

			result *= buffer[i + 1];
			break;

		case 13:

			result /= buffer[i + 1];
			break;

		}//end switch

	}

	// Now we calculate the fitness. First check to see if a solution has been found
	// and assign an arbitarily high fitness score if this is so.

	if (result == (float)target_value)
	{
		cout << "Result = " << result;
		return 999.0f;
	}
	else

		return 1 / (float)fabs((double)(target_value - result));
	//  return result;
}

//---------------------------------PrintChromo---------------------------------------
//
// decodes and prints a chromo to screen
//-----------------------------------------------------------------------------------
void PrintChromo(string bits)
{
	//holds decimal values of gene sequence
	int buffer[(int)(CHROMO_LENGTH / GENE_LENGTH)];

	//parse the bit string
	int num_elements = ParseBits(bits, buffer);

	for (int i = 0; i < num_elements; i++)
	{
		PrintGeneSymbol(buffer[i]);
	}

	return;
}

//--------------------------------------PrintGeneSymbol-----------------------------
//  
//  given an integer this function outputs its symbol to the screen 
//----------------------------------------------------------------------------------
void PrintGeneSymbol(int val)
{
	if (val < 10)

		cout << val << " ";

	else
	{
		switch (val)
		{

		case 10:

			cout << "+";
			break;

		case 11:

			cout << "-";
			break;

		case 12:

			cout << "*";
			break;

		case 13:

			cout << "/";
			break;

		}//end switch

		cout << " ";
	}

	return;
}

//------------------------------------Mutate---------------------------------------
//
//  Mutates a chromosome's bits dependent on the MUTATION_RATE
// if 1 becomes 0, if 0 becomes 1
//-------------------------------------------------------------------------------------
void Mutate(string &bits)
{
	for (unsigned int i = 0; i < bits.length(); i++)
	{
		if (RANDOM_NUM < MUTATION_RATE)
		{
			if (bits.at(i) == '1')

				bits.at(i) = '0';

			else

				bits.at(i) = '1';
		}
	}

	return;
}

//---------------------------------- Crossover ---------------------------------------
//
//  Dependent on the CROSSOVER_RATE this function selects a random point along the 
//  lenghth of the chromosomes and swaps all the  bits after that point.
//------------------------------------------------------------------------------------
void Crossover(string &offspring1, string &offspring2)
{
	//dependent on the crossover rate
	if (RANDOM_NUM < CROSSOVER_RATE)
	{
		//create a random crossover point
		int crossover = (int)(RANDOM_NUM * CHROMO_LENGTH); //give us a location

		string t1 = offspring1.substr(0, crossover) + offspring2.substr(crossover, CHROMO_LENGTH);
		string t2 = offspring2.substr(0, crossover) + offspring1.substr(crossover, CHROMO_LENGTH);
		//mix the chromosome of mom and dad's, using the random number
		// we use two parent to create two children to maintain the constant number of the chromosomes

		offspring1 = t1; offspring2 = t2;
	}
}


//--------------------------------Roulette-------------------------------------------
//
//  selects a chromosome from the population via roulette wheel selection
//------------------------------------------------------------------------------------
// the replacement here is randomly selected, but we want to keep the good genes and replace the bad genes

string Roulette(int total_fitness, vector<chromo_typ> Population)
{
	//generate a random number between 0 & total fitness count
	float Slice = (float)(RANDOM_NUM * total_fitness);

	//go through the chromosones adding up the fitness so far
	float FitnessSoFar = 0.0f;

	for (int i = 0; i < POP_SIZE; i++)
	{
		FitnessSoFar += Population[i].fitness;

		//if the fitness so far > random number return the chromo at this point
		if (FitnessSoFar >= Slice)
			// we select that chromosome

			return Population[i].bits;
	}

	return "";
}

//--------------------------------Relation Operator Overload-------------------------------------------

bool operator <(const chromo_typ& chromo1, const chromo_typ& chromo2) {
	if (chromo1.fitness < chromo2.fitness) {
		return true;
	}
	return false;
}

bool operator >(const chromo_typ& chromo1, const chromo_typ& chromo2) {
	if (chromo1.fitness > chromo2.fitness) {
		return true;
	}
	return false;
}