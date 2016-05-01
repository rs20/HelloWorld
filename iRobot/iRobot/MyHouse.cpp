#include "MyHouse.h"

int MyHouse::distanceFromDocking() {
	return 0;
}

void MyHouse::updateArea(Cell cell, SensorInformation si, char type) {
	// first, check if in bounds, if not -> resize vector
	// if robot went out of bounds to the left -> add 2 more columns to the left, and so on...
	if (cell.row < 0) {
		// add 2 more rows at the top
		int old_size = rows * cols;
		rows += 2;
		matrix.resize(rows * cols);
		std::copy(matrix.begin(), matrix.end() - 2 * cols, matrix.begin() + 2 * cols);
		// fill new cells as 'N' -> unknown
		for (std::vector<char>::iterator it = matrix.begin(); it != matrix.begin() + 2 * cols; it++)
			*it = 'N';
		docking = { docking.row + 2, docking.col };
		robot = { robot.row + 2, robot.col };
	}
	else if (cell.row >= rows) {
		// add 2 more rows at the bottom
		int old_size = rows * cols;
		rows += 2;
		matrix.resize(rows * cols);
		// fill new cells as 'N' -> unknown
		for (std::vector<char>::iterator it = matrix.end() - 2 * cols; it != matrix.end(); it++)
			*it = 'N';
	}
	else if (cell.col < 0) {
		// add 2 more columns to the left
		int old_size = rows * cols;
		std::vector<char> temp = std::vector<char>(old_size);
		std::copy(matrix.begin(), matrix.end(), temp);
		cols += 2;
		matrix.resize(rows * cols);
		for (int col = 0; col < cols - 2; col++) {
			matrix[rows * col] = 'N';
			matrix[rows * col + 1] = 'N';
			std::copy(temp.begin() + rows * col, temp.begin() + rows * col + (cols - 2), matrix.begin() + rows * col + 2);
		}
		docking = { docking.row, docking.col + 2 };
		robot = { robot.row, robot.col + 2 };
	}
	else if (cell.col >= cols) {
		// add 2 more columns to the right
		int old_size = rows * cols;
		std::vector<char> temp = std::vector<char>(old_size);
		std::copy(matrix.begin(), matrix.end(), temp);
		cols += 2;
		matrix.resize(rows * cols);
		for (int col = 0; col < cols - 2; col++) {
			std::copy(temp.begin() + rows * col, temp.begin() + rows * col + (cols - 2), matrix.begin() + rows * col);
			matrix[rows * col + (cols - 2)] = 'N';
			matrix[rows * col + (cols - 1)] 
	}
	updateCell(cell, type);
}

void MyHouse::updateCell(Cell cell, char type) {
	matrix[cell.row * rows + cell.col] = type;
}