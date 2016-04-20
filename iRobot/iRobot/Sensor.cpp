//#include "stdafx.h"

#include "Sensor.h"

Sensor::Sensor(House *h) {
	house = h;
}
SensorInformation Sensor::sense() const {
	SensorInformation si;
	char dirt = house->matrix[house->robot.row][house->robot.col];
	if (dirt >= '0' && dirt <= '9')
		si.dirtLevel = dirt - '0';
	else // dirt = ' ' / 'D' (0 dirt in docking station)
		si.dirtLevel = 0;
	for (int i = 0; i <= 3; i++)
		si.isWall[i] = false;
	// east
	if (house->matrix[house->robot.row][house->robot.col + 1] == 'W')
		si.isWall[0] = true;
	// west
	if (house->matrix[house->robot.row][house->robot.col - 1] == 'W')
		si.isWall[1] = true;
	// south
	if (house->matrix[house->robot.row + 1][house->robot.col] == 'W')
		si.isWall[2] = true;
	// north
	if (house->matrix[house->robot.row - 1][house->robot.col] == 'W')
		si.isWall[3] = true;
	return si;
}