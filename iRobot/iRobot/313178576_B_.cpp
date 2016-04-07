
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
Second Algorithm B:
1. rememebrs the path to the docking station.
2. remembers where the docking station is, and how to return to it.
3. hence, is able to reset path to docking station if entered docking station once again
4. hence, knows if the battery was charged.
5. logic of steps: - if current spot has dirt in it, then stay
                   - else, go to the first direction available
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

// Pro comparing to alg. A: knows where the docking station is compared to it's current location at each step


class _313178576_B : public AbstractAlgorithm
{
private:
	const AbstractSensor* sensor;
	int moreSteps;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	bool ending = false;
	list<Direction> path; // remember path in order to return to docking station
	int distanceToDocking = 0;
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
		// first, if started the move from the docking station -> charge battery
		if (cell.row == docking.row && cell.col == docking.col)
			curBattery += batteryRechargeRate;
		Direction step;
		// go back to docking station
		if (ending) {
			// reached docking, waiting for end of game
			if (path.empty())
				step = Direction::Stay;
			// get the last move made
			else {
				step = path.back();
				path.pop_back();
			}
			distanceToDocking--;
		}
		else if (shouldReturnDocking(moreSteps, distanceToDocking, curBattery, batteryConsumptionRate)) {
			ending = true;
			// reached docking, waiting for end of game
			if (path.empty())
				step = Direction::Stay;
			// get the last move made
			else {
				step = path.back();
				path.pop_back();
			}
			distanceToDocking--;
		}
		// continue cleaning
		else {
			SensorInformation si = sensor->sense();
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

				// if moved (!= stay) -> add move to path and increment distance to docking
				if (step != Direction::Stay) {
					// if step is the opposite of the last move -> remove both moves (2 last moves) from path list
					if (oppositeMove(step, path.back())) {
						path.pop_back();
						distanceToDocking--;
					}
					else {
						path.push_back(step);
						distanceToDocking++;
					}
				}

				// if returned to docking station: empty path and distance to docking
				if (cell.row == docking.row && cell.col == docking.col) {
					path.empty();
					distanceToDocking = 0;
				}
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

	bool shouldReturnDocking(int moreStepsAvailable, int distanceToDocking, int curBattery, int batteryConsumptionRate) {
		int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
		movesToMake = MIN(movesToMake, moreSteps);
		// ___
		//|_|_|
		//|D|R|
		// robot distance to docking is 1, but has 2 more moves to make
		// if it will go north -> he won't be able to return to the docking station
		if (distanceToDocking == movesToMake || distanceToDocking == movesToMake + 1)
			return true;
		return false;
	}
	bool oppositeMove(Direction d1, Direction d2) {
		if (d1 == Direction::East && d2 == Direction::West ||
			d1 == Direction::West && d2 == Direction::East ||
			d1 == Direction::South && d2 == Direction::North ||
			d1 == Direction::North && d2 == Direction::South)
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
};