
#include "Cell.h"
#include "Direction.h"
#include "SensorInformation.h"
#include <algorithm>
#include <vector>

/*
*** MyHouse is the robot's view of the house ***
characters and meanings:
1. 'W' - wall
2. 'D' - docking station
3. digits '0'-'9' - dirt
4. space ' ' - no dirt (== '0')
5. 'X' - unknown
6. 'N' - not a wall, but unknown dirt level
-> at first, the matrix is filled with 'X' except the docking station
*/

class MyHouse {
	// initialized as 3X3 matrix
	int rows = 3;
	int cols = 3;
	// relative position of robot and docking at start = center of matrix
	Cell docking = { 1, 1 };
	Cell robot = { 1, 1 };
	std::vector<char> matrix = std::vector<char>(3 * 3, 'X'); // size = rows * cols
public:
	MyHouse() { updateCell(docking, 'D'); }
	MyHouse(const MyHouse&) = delete;
	MyHouse& operator=(const MyHouse&) = delete;
	void resetHouse(); // called when the algorithm starts working on a new house
	int distanceFromDocking();
	Cell getDocking() { return docking; }
	Cell getRobot() { return robot; }
	void updateRobot(Direction direction);
private:
	char getCell(Cell cell) { return matrix[cols * cell.row + cell.col]; }
	void updateCell(Cell cell, char type);
	void updateArea(Cell cell, SensorInformation si);
};