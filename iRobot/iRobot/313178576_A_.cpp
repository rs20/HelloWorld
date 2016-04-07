
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
First Algorithm A:
1. rememebrs the path to the docking station.
2. does not know where the docking station is, but only how to return to it.
3. hence, is not able to reset path to docking station if entered docking station not on purpose
4. hence, does not know if the battery was charged.
5. logic of steps: - if current spot has dirt in it, then stay
                   - else, go to the first direction available
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

class _313178576_A : public AbstractAlgorithm
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
			}
		}
		moreSteps--;
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
};