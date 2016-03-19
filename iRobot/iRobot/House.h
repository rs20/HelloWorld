// House struct
#include <string>

typedef struct House
{
	std::string houseName;
	std::string houseDescription;
	char** matrix;
	int rows;
	int cols;
	int robotRow;
	int robotCol;
	int dockingRow;
	int dockingCol;
	int initialSumOfDirt;
	int sumOfDirt;
} House;