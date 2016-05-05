
#include "Cell.h"
#include "Direction.h"
#include "SensorInformation.h"
#include <map>

/*
*** MyHouse is the robot's view of the house ***
characters and meanings:
1. 'W' - wall
2. 'D' - docking station
3. digits '0'-'9' - dirt
4. space ' ' - no dirt (== '0') (when there's 0 dirt -> I will use ' ' instead of '0')
5. 'X' - not a wall, but unknown dirt level
*/

class MyHouse {
	int rows = 1;
	int cols = 1;
	// relative position
	Cell docking = { 0, 0 };
	Cell robot = { 0, 0 };
	std::map<Cell, char> house = { { docking, 'D' } };
public:
	MyHouse() {}
	MyHouse(const MyHouse&) = delete;
	MyHouse& operator=(const MyHouse&) = delete;
	void resetHouse(); // called when the algorithm starts working on a new house
	int distanceFromDocking();
	Cell getDocking() { return docking; }
	Cell getRobot() { return robot; }
	void updateRobot(Direction direction);
private:
	void updateCell(Cell cell, char type);
	void updateArea(Cell cell, SensorInformation si);
};