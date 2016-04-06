// Sensor.cpp
#include "stdafx.h"

#include "AbstractSensor.h"

#ifndef __HOUSE_H
#define __HOUSE_H
#include "House.h"
#endif

class Sensor : public AbstractSensor
{
private:
	House* house;
public:
	// pass the house to the sensor (by reference)
	Sensor(House *h) {
		house = h;
	}
	virtual SensorInformation sense() const override {
		SensorInformation si;
		char dirt = house->matrix[house->robotRow][house->robotCol];
		if (dirt >= '0' && dirt <= '9')
			si.dirtLevel = dirt - '0';
		else // dirt = ' ' / 'D' (0 dirt in docking station)
			si.dirtLevel = 0;
		for (int i = 0; i <= 3; i++)
			si.isWall[i] = false;
		// east
		if (house->matrix[house->robotRow][house->robotCol + 1] == 'W')
			si.isWall[0] = true;
		// west
		if (house->matrix[house->robotRow][house->robotCol - 1] == 'W')
			si.isWall[1] = true;
		// south
		if (house->matrix[house->robotRow + 1][house->robotCol] == 'W')
			si.isWall[2] = true;
		// north
		if (house->matrix[house->robotRow - 1][house->robotCol] == 'W')
			si.isWall[3] = true;
		return si;
	}
};