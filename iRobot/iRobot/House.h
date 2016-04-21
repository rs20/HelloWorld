#ifndef __HOUSE__H
#define __HOUSE__H

// House struct
#include <string>

#include "Cell.h"


typedef struct House
{
	// information based on .house file
	std::string houseFileName;
	std::string houseDescription;
	int maxSteps;
	int rows;
	int cols;
	char** matrix;
	// information the program figured out
	Cell robot;
	Cell docking;
	int initialSumOfDirt;
	int sumOfDirt;
	bool isValidHouse; // flag if the house is formatted properly or not (and so should be freed)
	std::string error;
	bool ifToFree;
} House;

#endif //__HOUSE__H