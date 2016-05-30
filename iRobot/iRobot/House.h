#ifndef __HOUSE__H
#define __HOUSE__H

// House struct
#include <string>

#include "Cell.h"
#include "MakeUnique.h"

class House
{
public:
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
	int picCounter = 0;

	int montage(const std::string& algoName, vector<string>& videoErrors);
};

#endif //__HOUSE__H