
#include "Cell.h"
#include "SensorInformation.h"
#include <algorithm>
#include <vector>

class MyHouse {
	// initialized as 5X5 matrix
	int rows = 5;
	int cols = 5;
	// relative position of robot and docking at start = center of matrix
	Cell docking = { 2, 2 };
	Cell robot = { 2,2 };
	// matrix at initialization filled with 'N' to represent 'unknown'
	std::vector<char> matrix = std::vector<char>(5 * 5, 'N'); // size = rows * cols
public:
	MyHouse() { updateCell(docking, 'D'); }
	int distanceFromDocking();
	Cell getDocking() { return docking; }
	Cell getRobot() { return robot; }
private:
	void updateArea(Cell cell, SensorInformation si, char type);
	void updateCell(Cell cell, char type);
};