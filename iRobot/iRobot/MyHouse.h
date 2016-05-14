
#include <list>
//#include <unordered_map>
#include <map>
#include <queue>
#include <set>

//#include "HashCell.h"
#include "Cell.h"
#include "Direction.h"
#include "SensorInformation.h"

/*
*** MyHouse is the robot's view of the house ***
characters and meanings:
1. 'W' - wall
2. 'D' - docking station
3. digits '0'-'9' - dirt
4. space ' ' - no dirt (== '0') (when there's 0 dirt -> I will use ' ' instead of '0')
5. 'X' - not a wall, but unknown dirt level

I tried using unordered_map with some hash function to speed things up with no success.
so, currently, we're using a regular map
*/

class MyHouse {
	// relative position
	Cell docking = { 0, 0 };
	Cell robot = { 0, 0 };
	std::map<Cell, char> house = { {docking, 'D'} };
	//std::unordered_map<Cell, char, HashCell> house = { { docking, 'D' } };
public:
	void resetHouse(); // called when the algorithm starts working on a new house
	std::list<Direction> BFS(Cell start, Cell &end, char type);
	//std::list<Direction> IDDFS(Cell start, Cell &end, int depth);
	Cell getDocking() { return docking; }
	Cell getRobot() { return robot; }
	void updateRobot(Direction direction);
	bool hasCell(Cell cell);
	char getCell(Cell cell) { return house.at(cell); }
	void updateRobotArea(SensorInformation si);
private:
	void updateCell(Cell cell, char type);
	//bool DLS(std::list<Direction> &path, Cell start, Cell &end, int depth);
};