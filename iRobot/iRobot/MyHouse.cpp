#include "MyHouse.h"

int MyHouse::distanceFromDocking()
{
	return 0;
}

void MyHouse::resetHouse()
{
	rows = 3;
	cols = 3;
	docking = { 1, 1 };
	robot = { 1, 1 };
	matrix = std::vector<char>(3 * 3, 'X');
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
	matrix[cell.row * rows + cell.col] = type;
}

// the algorithm should call this function right after calling 'sense' to the sensor
void MyHouse::updateArea(Cell cell, SensorInformation si)
{
	// first, check if in bounds, if not -> resize vector
	// if robot stands on an edge of the rectangle -> enlarge the house
	// (if stood on the left edge -> add 1 more column(s) to the left, and so on...)
	if (cell.row == 0) {
		// add 1 more row(s) at the top
		rows += 1;
		matrix.resize(rows * cols);
		std::copy(matrix.begin(), matrix.end() - 1 * cols, matrix.begin() + 1 * cols);
		// fill new cells as 'X' -> unknown
		for (std::vector<char>::iterator it = matrix.begin(); it != matrix.begin() + 1 * cols; it++)
			*it = 'X';
		docking.row += 1;
		robot.row += 1;
		cell.row += 1;
	}
	else if (cell.row == rows - 1) {
		// add 1 more row(s) at the bottom
		rows += 1;
		matrix.resize(rows * cols);
		// fill new cells as 'X' -> unknown
		for (std::vector<char>::iterator it = matrix.end() - 1 * cols; it != matrix.end(); it++)
			*it = 'X';
	}
	else if (cell.col == 0) {
		// add 1 more column(s) to the left
		int old_size = rows * cols;
		std::vector<char> temp = std::vector<char>(old_size);
		std::copy(matrix.begin(), matrix.end(), temp.begin());
		cols += 1;
		matrix.resize(rows * cols);
		for (int col = 0; col < cols - 1; col++) {
			matrix[rows * col] = 'X';
			std::copy(temp.begin() + rows * col, temp.begin() + rows * col + (cols - 1), matrix.begin() + rows * col + 1);
		}
		docking.col += 1;
		robot.col += 1;
		cell.col += 1;
	}
	else if (cell.col == cols - 1) {
		// add 1 more column(s) to the right
		int old_size = rows * cols;
		std::vector<char> temp = std::vector<char>(old_size);
		std::copy(matrix.begin(), matrix.end(), temp.begin());
		cols += 1;
		matrix.resize(rows * cols);
		for (int col = 0; col < cols - 1; col++) {
			std::copy(temp.begin() + rows * col, temp.begin() + rows * col + (cols - 1), matrix.begin() + rows * col);
			matrix[rows * col + (cols - 1)] = 'X';
		}
	}
	
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
	if (getCell(east) == 'X') {
		if (si.isWall[0])
			updateCell(east, 'W');
		else
			updateCell(east, 'N');
	}
	if (getCell(west) == 'X') {
		if (si.isWall[1])
			updateCell(west, 'W');
		else
			updateCell(west, 'N');
	}
	if (getCell(south) == 'X') {
		if (si.isWall[2])
			updateCell(south, 'W');
		else
			updateCell(south, 'N');
	}
	if (getCell(north) == 'X') {
		if (si.isWall[3])
			updateCell(north, 'W');
		else
			updateCell(north, 'N');
	}
}