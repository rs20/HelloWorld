
#include "stdafx.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#ifndef __CELL_H
#define __CEL_H
#include "Cell.h"
#endif
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
                   - else, go to the first direction different than the last step made that is available
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

// Pro comparing to alg. A: knows where the docking station is compared to it's current location at each step


class _313178576_B : public AbstractAlgorithm
{
private:
	//list<const AbstractSensor*> sensors;
	const AbstractSensor* sensor;
	int moreSteps = -1;
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
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override {
		//sensors.push_back(&s);
		sensor = &s;
		moreSteps = -1;
		curBattery = batteryCapacity;
		ending = false;
		path.clear();
		distanceToDocking = 0;
		cell = { 0, 0 };
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
				step = oppositeMove(path.back());
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
				step = oppositeMove(path.back());
				path.pop_back();
			}
			distanceToDocking--;
		}
		// continue cleaning
		else {
			//SensorInformation si = sensors.back()->sense();
			SensorInformation si = sensor->sense();

			// move into cell without staying does clean the spot anyway
			if (si.dirtLevel > 1)
				step = Direction::Stay;
			else {
				int directions = 0;
				// count available moves
				for (int i = 0; i < 4; i++)
					directions += (si.isWall[i]) ? 0 : 1;

				// if more than one available -> do not repeat last move
				Direction lastStep;
				if (!path.empty() && directions > 1)
					lastStep = path.back();

				// pick first available direction to move to different than the last step made
				// choose last step only if it is the only available move
				if (!path.empty() && directions > 1) {
					if (si.isWall[0] == false && oppositeMove(lastStep) != Direction::East)
						step = Direction::East;
					else if (si.isWall[1] == false && oppositeMove(lastStep) != Direction::West)
						step = Direction::West;
					else if (si.isWall[2] == false && oppositeMove(lastStep) != Direction::South)
						step = Direction::South;
					else if (si.isWall[3] == false && oppositeMove(lastStep) != Direction::North)
						step = Direction::North;
					else
						step = Direction::Stay;
				}
				else {
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

				// if moved (!= stay) -> add move to path and increment distance to docking
				if (step != Direction::Stay) {
					// if step is the opposite of the last move -> remove both moves (2 last moves) from path list
					if (!path.empty() && isOppositeMove(step, path.back())) {
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
					path.clear();
					distanceToDocking = 0;
				}
			}
		}
		updateSpot(step);
		if (moreSteps != -1)
			moreSteps--;
		// consume battery only if did not start the move from the docking station
		if (cell.row != docking.row || cell.col != docking.col)
			curBattery -= batteryConsumptionRate;
		return step;
	}
	virtual void aboutToFinish(int stepsTillFinishing) override {
		moreSteps = stepsTillFinishing;
	}

private:
	bool shouldReturnDocking(int moreStepsAvailable, int distanceToDocking, int curBattery, int batteryConsumptionRate) {
		int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
															   // if more steps is up to date -> take into consideration
		if (moreSteps != -1)
			movesToMake = MIN(movesToMake, moreSteps);
		// ___
		//|_|_|
		//|D|R|
		// robot distance to docking is 1, but has 2 more moves to make
		// if it will go north -> he won't be able to return to the docking station
		if (distanceToDocking >= movesToMake - 1)
			return true;
		return false;
	}
	bool isOppositeMove(Direction d1, Direction d2) {
		if (d1 == Direction::East && d2 == Direction::West ||
			d1 == Direction::West && d2 == Direction::East ||
			d1 == Direction::South && d2 == Direction::North ||
			d1 == Direction::North && d2 == Direction::South)
			return true;
		return false;
	}
	Direction oppositeMove(Direction d) {
		// 0->east, 1->west, 2->south, 3->north, 4->stay
		switch (d)
		{
		case static_cast<Direction>(0) :
			return Direction::West;
		case static_cast<Direction>(1) :
			return Direction::East;
		case static_cast<Direction>(2) :
			return Direction::North;
		case static_cast<Direction>(3) :
			return Direction::South;
		default:
			return Direction::Stay;
		}
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