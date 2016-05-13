#ifndef __HOUSE__H
#define __HOUSE__H

// House struct
#include <string>

#include "Cell.h"
#include "MakeUnique.h"

typedef struct House
{
	// information based on .house file
	std::string houseFileName;
	std::string houseDescription;
	int maxSteps;
	int rows;
	int cols;
	std::unique_ptr<std::unique_ptr<char[]>[]> matrix;
	// information the program figured out
	Cell robot;
	Cell docking;
	int initialSumOfDirt;
	int sumOfDirt;
} House;

#endif //__HOUSE__H