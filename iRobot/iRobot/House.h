// House struct
#include <string>
#include "Cell.h"

typedef struct House
{
	// inofrmation based on .house file
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
	bool isValidHouse; // flag if the house is formatted properly or not
	std::string error;
} House;