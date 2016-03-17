#include "stdafx.h"

#include "AbstractSensor.h"

class Sensor : public AbstractSensor
{
private:
	House house;
public:
	// pass the house to the sensor (by reference)
	Sensor() {}
	Sensor(House &h) {
		house = h;
	}
	SensorInformation sense() const {
		SensorInformation si;
		char dirt = house.matrix[house.robotRow][house.robotCol];
		if (dirt >= '0' && dirt <= '9')
			si.dirtLevel = dirt - '0';
		else // dirt = ' '
			si.dirtLevel = 0;
		if (house.matrix[house.robotRow][house.robotCol + 1] == 'W')
			si.isWall[0] = true;
		if (house.matrix[house.robotRow][house.robotCol - 1] == 'W')
			si.isWall[1] = true;
		if (house.matrix[house.robotRow + 1][house.robotCol] == 'W')
			si.isWall[2] = true;
		if (house.matrix[house.robotRow - 1][house.robotCol] == 'W')
			si.isWall[3] = true;
		return si;
	}
};