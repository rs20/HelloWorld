
#include "stdafx.h"
#ifndef __DIRECTION_H
#define __DIRECTION_H
#include "Direction.h"
#endif
#ifndef __ABSTRACT_ALGORITHM_H
#define __ABSTRACT_ALGORITHM_H
#include "AbstractAlgorithm.h"
#endif
#ifndef __CELL_H
#define __CELL_H
#include "Cell.h"
#endif
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
	//list<const AbstractSensor*> sensors;
	const AbstractSensor* sensor;
	int moreSteps = -1;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	bool ending = false;
	// this is a relative position of the docking since the algorithm does not know the house dimensions / docking station's spot
	Cell docking = { 0, 0 };
	Cell cell = { 0, 0 };
	// remember last step
	Direction lastStep = Direction::Stay;
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override {
		//sensors.push_back(&s);
		sensor = &s;
		moreSteps = -1;
		curBattery = batteryCapacity;
		ending = false;
		cell = { 0, 0 };
		docking = { 0, 0 };
		lastStep = Direction::Stay;
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
		//SensorInformation si = sensors.back()->sense();
		SensorInformation si = sensor->sense();
		// first, if started the move from the docking station -> charge battery
		if (cell.row == docking.row && cell.col == docking.col)
			curBattery += batteryRechargeRate;
		curBattery = MIN(curBattery, batteryCapacity);

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
			// move into cell without staying does clean the spot anyway
			if (si.dirtLevel > 1)
				step = Direction::Stay;
			else {
				int directions = 0;
				// count available moves
				for (int i = 0; i < 4; i++)
					directions += (si.isWall[i]) ? 0 : 1;
				// if more than one available -> do not repeat last move
	
				// pick first available direction to move to different than the last step made
				// choose last step only if it is the only available move
				if (directions > 1) {
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
			}
		}
		if (step != Direction::Stay)
			lastStep = step;

		// consume battery only if did not start the move from the docking station
		if (cell.row != docking.row || cell.col != docking.col)
			curBattery -= batteryConsumptionRate;

		updateSpot(step);
		if (moreSteps != -1)
			moreSteps--;

		return step;
	}
	virtual void aboutToFinish(int stepsTillFinishing) override {
		moreSteps = stepsTillFinishing;
	}

private:
	bool shouldReturnDocking(int moreStepsAvailable, int curBattery, int batteryConsumptionRate) {
		int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
		if (moreSteps != -1)
			movesToMake = MIN(movesToMake, moreSteps);
		// ___
		//|_|_|
		//|D|R|
		// robot distance to docking is 1, but has 2 more moves to make
		// if it will go north -> he won't be able to return to the docking station
		if (distanceToDocking() >= movesToMake - 1)
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