
#include "stdafx.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Sensor.cpp"
#include <stdlib.h>
#include <list>

#ifndef __MIN_
#define __MIN
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

/*
Third Algorithm C:
1. adventurous algorithm: will not return to the docking station in the same order it left it.
2. it just remembers the relative location of the docking station and the relative spot of the robot, and tries to return to it in a straight line.
3. if the algorithm wants to return to the docking station, say the direction of the docking station relative to current cell is nort west,
   the algorithm will make only north/west steps.
   according to the precedence in Direction.h (west < north); it will try making all the west steps, 
   and only after that (stuck / got to the same column as the docking station) it will make north steps
   and if stuck again, back to west steps and so on until reaches docking station
   if stuck on both direction, just stay at the same spot
*/

// Pro: may be more efficient
// Con: probably will not return to the docking station successfully


class _313178576_C : public AbstractAlgorithm
{
private:
	const AbstractSensor* sensor;
	int moreSteps;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	bool ending = false;
	// this is a relative position of the docking since the algorithm does not know the house dimensions / docking station's spot
	Cell docking = { 0, 0 };
	Cell cell = { 0, 0 };
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	virtual void setSensor(const AbstractSensor& s) override {
		sensor = &s;
	}
	// was not specified in AbstractAlgorithm
	// however, since the config.ini file does not include max steps, but each house is
	// the algorithm needs a way to find out how many more steps it may commit
	void setMaxSteps(int maxSteps) {
		moreSteps = maxSteps;
	}
	virtual void setConfiguration(map<string, int> config) override {
		map<string, int>::iterator it;
		it = config.find("BatteryCapacity");
		batteryCapacity = it->second;
		curBattery = batteryCapacity;
		it = config.find("BatteryConsumptionRate");
		batteryConsumptionRate = it->second;
		it = config.find("BatteryRechargeRate");
		batteryRechargeRate = it->second;
	}
	virtual Direction step() override {
		SensorInformation si = sensor->sense();
		// first, if started the move from the docking station -> charge battery
		if (cell.row == docking.row && cell.col == docking.col)
			curBattery += batteryRechargeRate;
		Direction step;
		// go back to docking station
		if (ending) {
			// reached docking, waiting for end of game
			if (cell.row == docking.row && cell.col == docking.col)
				step = Direction::Stay;
			else
				step = goHome(si);
		}
		else if (shouldReturnDocking(moreSteps, curBattery, batteryConsumptionRate)) {
			ending = true;
			// reached docking, waiting for end of game
			if (cell.row == docking.row && cell.col == docking.col)
				step = Direction::Stay;
			else
				step = goHome(si);
		}
		// continue cleaning
		else {
			if (si.dirtLevel > 0)
				step = Direction::Stay;
			else {
				// pick first available direction to move to
				if (si.isWall[0] == false)
					step = Direction::East;
				else if (si.isWall[1] == false)
					step = Direction::West;
				else if (si.isWall[2] == false)
					step = Direction::South;
				else if (si.isWall[3] == false)
					step = Direction::North;
				else
					step = Direction::Stay;
			}
		}
		updateSpot(step);
		moreSteps--;
		// consume battery only if did not start the move from the docking station
		if (cell.row != docking.row || cell.col != docking.col)
			curBattery -= batteryConsumptionRate;
		return step;
	}
	virtual void aboutToFinish(int stepsTillFinishing) override {
		moreSteps = stepsTillFinishing;
	}

	bool shouldReturnDocking(int moreStepsAvailable, int curBattery, int batteryConsumptionRate) {
		int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
		movesToMake = MIN(movesToMake, moreSteps);
		// ___
		//|_|_|
		//|D|R|
		// robot distance to docking is 1, but has 2 more moves to make
		// if it will go north -> he won't be able to return to the docking station
		if (distanceToDocking() == movesToMake || distanceToDocking() == movesToMake + 1)
			return true;
		return false;
	}
	void updateSpot(Direction step) {
		switch (step)
		{
		case static_cast<Direction>(0) :
			cell.col++;
			break;
		case static_cast<Direction>(1) :
			cell.col--;
			break;
		case static_cast<Direction>(2) :
			cell.row++;
			break;
		case static_cast<Direction>(3) :
			cell.row--;
			break;
		default:
			break;
			// do nothing for 'Stay'
		}
	}
	int distanceToDocking() {
		// distance is the distance in rows + distance in columns
		return abs(cell.row - docking.row) + abs(cell.col - docking.col);
	}
	Direction goHome(SensorInformation si) {
		Direction step;
		// reminder: order of precedence: (1) east (2) west (3) south (4) north (5) stay
		if (docking.col > cell.col && si.isWall[0] == false)
			step = Direction::East;
		else if (docking.col < cell.col && si.isWall[1] == false)
			step = Direction::West;
		else if (docking.row < cell.row && si.isWall[2] == false)
			step = Direction::South;
		else if (docking.row > cell.row && si.isWall[3] == false)
			step = Direction::North;
		else
			step = Direction::Stay;
		return step;
	}
};