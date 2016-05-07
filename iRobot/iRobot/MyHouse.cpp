#include "MyHouse.h"

void MyHouse::resetHouse()
{
	docking = { 0, 0 };
	robot = { 0, 0 };
	house.clear();
	house = { { docking, 'D' } };
}

// return shortest path (list) from robot to the closest cell with char type
// when type=='D' -> return shortest path to the docking station
// returns empty list when not found (for example: cleaned the whole house -> no 'X' would be found)
// FOR ALGORITHM 1: (may change it)
//	when type == 'X', BFS will match type to '1'-'9' + 'X'
std::list<Direction> MyHouse::BFS(char type)
{
	// the queue is pairs of <list of directions from start (robot), last Cell in track>
	std::queue<std::pair<std::list<Direction>, Cell>> q;
	std::list<Direction> start;
	q.push({ start, robot });
	std::set<Cell> visited;
	visited.insert(robot);
	while (q.size() > 0) {
		std::list<Direction> list = q.front().first;
		Cell cell = q.front().second;
		q.pop();
		Cell east = { cell.row, cell.col + 1 };
		Cell west = { cell.row, cell.col - 1 };
		Cell south = { cell.row + 1, cell.col };
		Cell north = { cell.row - 1, cell.col };
		// if cell is known to house map, it wasn't visited and is not a wall, create a new list with it
		if (hasCell(east) && (visited.count(east) == 0) && (house.at(east) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::East);
			if (type == 'X') {
				char t = getCell(east);
				if (((t > '0') && (t <= '9')) || t == 'X')
					return newList;
			}
			else if (getCell(east) == type)
				return newList;
			q.push({ newList, east });
			visited.insert(east);
		}
		if (hasCell(west) && (visited.count(west) == 0) && (house.at(west) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::West);
			if (type == 'X') {
				char t = getCell(west);
				if (((t > '0') && (t <= '9')) || t == 'X')
					return newList;
			}
			else if (getCell(west) == type)
				return newList;
			q.push({ newList, west });
			visited.insert(west);
		}
		if (hasCell(south) && (visited.count(south) == 0) && (house.at(south) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::South);
			if (type == 'X') {
				char t = getCell(south);
				if (((t > '0') && (t <= '9')) || t == 'X')
					return newList;
			}
			else if (getCell(south) == type)
				return newList;
			q.push({ newList, south });
			visited.insert(south);
		}
		if (hasCell(north) && (visited.count(north) == 0) && (house.at(north) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::North);
			if (type == 'X') {
				char t = getCell(north);
				if (((t > '0') && (t <= '9')) || t == 'X')
					return newList;
			}
			else if (getCell(north) == type)
				return newList;
			q.push({ newList, north });
			visited.insert(north);
		}
	}
	return start; // return empty list when no char type found
}

void MyHouse::updateRobot(Direction direction)
{
	if (direction == Direction::East)
		robot.col++;
	else if (direction == Direction::West)
		robot.col--;
	else if (direction == Direction::South)
		robot.row++;
	else if (direction == Direction::North)
		robot.row--;
}

bool MyHouse::hasCell(Cell cell)
{
	return (house.count(cell) > 0);
}

void MyHouse::updateCell(Cell cell, char type)
{
	house[cell] = type;
}

// the algorithm should call this function right after calling 'sense' to the sensor
void MyHouse::updateRobotArea(SensorInformation si)
{
	// update house according to SensorInformation
	// update current cell dirt level
	if (robot != docking) {
		// cleans 1 amount of dirt during the current step
		if (si.dirtLevel == 0 || si.dirtLevel == 1)
			updateCell(robot, ' ');
		else
			updateCell(robot, (si.dirtLevel - 1) + '0');
	}
	// update around if unknown
	Cell east = { robot.row, robot.col + 1 }, west = { robot.row, robot.col - 1 }, south = { robot.row + 1, robot.col }, north = { robot.row - 1, robot.col };
	if (!hasCell(east)) {
		if (si.isWall[0])
			updateCell(east, 'W');
		else
			updateCell(east, 'X');
	}
	if (!hasCell(west)) {
		if (si.isWall[1])
			updateCell(west, 'W');
		else
			updateCell(west, 'X');
	}
	if (!hasCell(south)) {
		if (si.isWall[2])
			updateCell(south, 'W');
		else
			updateCell(south, 'X');
	}
	if (!hasCell(north)) {
		if (si.isWall[3])
			updateCell(north, 'W');
		else
			updateCell(north, 'X');
	}
}