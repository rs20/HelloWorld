#include "MyHouse.h"

void MyHouse::resetHouse()
{
	docking = { 0, 0 };
	robot = { 0, 0 };
	house.clear();
	house = { { docking, 'D' } };
}

// return shortest path (list) from cell 'start' to the closest cell with char type and the last cell itself
// when type=='D' -> return shortest path to the docking station
// returns empty list when not found (for example: cleaned the whole house -> no 'X' would be found)
// FOR ALGORITHM 1: (may change it)
//	when type == 'X', BFS will match type to '1'-'9' + 'X'
std::list<Direction> MyHouse::BFS(Cell start, Cell &end, char type)
{
	// the queue is pairs of <list of directions from start (robot), last Cell in track>
	std::queue<std::pair<std::list<Direction>, Cell>> q;
	std::list<Direction> emptyPath;
	q.push({ emptyPath, start });
	std::set<Cell> visited;
	visited.insert(start);
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
				if (((t > '0') && (t <= '9')) || t == 'X') {
					end = east;
					return newList;
				}
			}
			else if (getCell(east) == type) {
				end = east;
				return newList;
			}
			q.push({ newList, east });
			visited.insert(east);
		}
		if (hasCell(west) && (visited.count(west) == 0) && (house.at(west) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::West);
			if (type == 'X') {
				char t = getCell(west);
				if (((t > '0') && (t <= '9')) || t == 'X') {
					end = west;
					return newList;
				}
			}
			else if (getCell(west) == type) {
				end = west;
				return newList;
			}
			q.push({ newList, west });
			visited.insert(west);
		}
		if (hasCell(south) && (visited.count(south) == 0) && (house.at(south) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::South);
			if (type == 'X') {
				char t = getCell(south);
				if (((t > '0') && (t <= '9')) || t == 'X') {
					end = south;
					return newList;
				}
			}
			else if (getCell(south) == type) {
				end = south;
				return newList;
			}
			q.push({ newList, south });
			visited.insert(south);
		}
		if (hasCell(north) && (visited.count(north) == 0) && (house.at(north) != 'W')) {
			std::list<Direction> newList = list;
			newList.push_back(Direction::North);
			if (type == 'X') {
				char t = getCell(north);
				if (((t > '0') && (t <= '9')) || t == 'X') {
					end = north;
					return newList;
				}
			}
			else if (getCell(north) == type) {
				end = north;
				return newList;
			}
			q.push({ newList, north });
			visited.insert(north);
		}
	}
	end = start;
	return emptyPath; // return empty list when no char type found
}


/*
1st try -> not successfull

std::list<Direction> MyHouse::IDDFS(Cell start, Cell &end, int depth)
{
	std::list<Direction> path;
	std::set<Cell> visited;
	for (int dep = depth; dep > 0; dep--) {
		bool found = DLS(path, visited, start, end, depth);
		if (found)
			return path;
		path.clear();
	}
	return path; // empty path
}

bool MyHouse::DLS(std::list<Direction> &path, std::set<Cell> visited, Cell cur, Cell &end, int depth)
{
	if ((depth == 0) && hasCell(cur) && ((getCell(cur) == 'X') || ((getCell(cur) > '0') && (getCell(cur) < '9')))) {
		end = cur;
		return true;
	}
	else if (depth > 0) {
		visited.insert(cur);
		Cell east = { cur.row, cur.col + 1 };
		if ((visited.count(east) == 0) && hasCell(east) && (getCell(east) != 'W') && DLS(path, visited, east, end, depth - 1)) {
			path.push_front(Direction::East);
			return true;
		}
		Cell west = { cur.row, cur.col - 1 };
		if ((visited.count(west) == 0) && hasCell(west) && (getCell(west) != 'W') && DLS(path, visited, west, end, depth - 1)) {
			path.push_front(Direction::West);
			return true;
		}
		Cell south = { cur.row + 1, cur.col };
		if ((visited.count(south) == 0) && hasCell(south) && (getCell(south) != 'W') && DLS(path, visited, south, end, depth - 1)) {
			path.push_front(Direction::South);
			return true;
		}
		Cell north = { cur.row - 1, cur.col };
		if ((visited.count(north) == 0) && hasCell(north) && (getCell(north) != 'W') && DLS(path, visited, north, end, depth - 1)) {
			path.push_front(Direction::North);
			return true;
		}
	}
	return false;
}
*/

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