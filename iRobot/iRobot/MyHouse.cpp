#include "MyHouse.h"

// return path (list) from robot to docking station
std::list<Direction> MyHouse::toDocking()
{
	std::list<Direction> list;
	return list;
}

void MyHouse::resetHouse()
{
	rows = 1;
	cols = 1;
	docking = { 0, 0 };
	robot = { 0, 0 };
	house.clear();
	house = { {docking, 'D'} };
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

void MyHouse::updateCell(Cell cell, char type)
{
	house[cell] = type;
}

// the algorithm should call this function right after calling 'sense' to the sensor
// assumes the robot's position is cell
void MyHouse::updateArea(Cell cell, SensorInformation si)
{
	// update house according to SensorInformation
	// update current cell dirt level
	if (si.dirtLevel > 0) {
		// cleans 1 amount of dirt during the current step
		if (si.dirtLevel == 1)
			updateCell(cell, ' ');
		else
			updateCell(cell, (si.dirtLevel - 1) + '0');
	}
	// update around if unknown
	Cell east = { cell.row, cell.col + 1 }, west = { cell.row, cell.col - 1 }, south = { cell.row + 1, cell.col }, north = { cell.row - 1, cell.col };
	if (house.count(east) == 0) {
		if (si.isWall[0])
			updateCell(east, 'W');
		else
			updateCell(east, 'X');
	}
	if (house.count(west) == 0) {
		if (si.isWall[1])
			updateCell(west, 'W');
		else
			updateCell(west, 'X');
	}
	if (house.count(south) == 0) {
		if (si.isWall[2])
			updateCell(south, 'W');
		else
			updateCell(south, 'X');
	}
	if (house.count(north) == 0) {
		if (si.isWall[3])
			updateCell(north, 'W');
		else
			updateCell(north, 'X');
	}
}