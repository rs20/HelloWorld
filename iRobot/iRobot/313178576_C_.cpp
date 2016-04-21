//#include "stdafx.h"

#include <stdlib.h>
#include <list>
#include "313178576_C_.h"
	
REGISTER_ALGORITHM(_313178576_C)

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
4. logic of steps: - if current spot has dirt in it, then stay
- else, go to the first direction (order of precedence: east->west->south->north) different than the last step made that is available
*/

// Pro: may be more efficient
// Con: probably will not return to the docking station successfully



void _313178576_C::setSensor(const AbstractSensor& s)
{
	//sensors.push_back(&s);
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	ending = false;
	cell = { 0, 0 };
	docking = { 0, 0 };
	lastStep = Direction::Stay;
}

void _313178576_C::setConfiguration(map<string, int> config)
{
	map<string, int>::iterator it;
	it = config.find("BatteryCapacity");
	batteryCapacity = it->second;
	curBattery = batteryCapacity;
	it = config.find("BatteryConsumptionRate");
	batteryConsumptionRate = it->second;
	it = config.find("BatteryRechargeRate");
	batteryRechargeRate = it->second;
}


Direction _313178576_C::step()
{
	//SensorInformation si = sensors.back()->sense();
	SensorInformation si = sensor->sense();
	// first, if started the move from the docking station -> charge battery
	if (cell.row == docking.row && cell.col == docking.col)
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	Direction step;
	// go back to docking station - get the last move made
	if (ending && (distanceToDocking() != 0)) {
		step = goHome(si);
	}
	else if (ending && (distanceToDocking() == 0) && !goOnCleaningAgain()) {
		// reached docking, waiting for end of game/recharge
		step = Direction::Stay;
		//distanceToDocking==0
	}
	else if (!ending && shouldReturnDocking()) {
		ending = true;
		// reached docking, waiting for end of game
		if (distanceToDocking() == 0)
			step = Direction::Stay;
		// continue going home
		else
			step = goHome(si);
	}
	// continue cleaning OR reaches here if already back in docking station but has enough battery to continue cleaning again
	else {
		ending = false;
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

void _313178576_C::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}



bool _313178576_C::shouldReturnDocking()
{
	int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
	if (moreSteps != -1)
		movesToMake = MIN(movesToMake, moreSteps);
	// ___
	//|_|_|
	//|D|R|
	// robot distance to docking is 1, but has 2 more moves to make
	// if it will go north -> he won't be able to return to the docking station
	// its '-2' because we don't want to return to the docking station exactly with empty battery -> this means we DIE.
	// so go back with extra for one move which means you will be able to continue after recharging yourself
	if (distanceToDocking() >= movesToMake - 2)
		return true;
	return false;
}


bool _313178576_C::goOnCleaningAgain()
{
	// don't know how many more moves yet -> check battery recharged enough
	if (moreSteps == -1) {
		// charged enough
		if (curBattery > batteryCapacity / 2)
			return true;
		return false;
	}
	// check both moreSteps and curBattery and decide according to both
	else {
		if ((curBattery > batteryCapacity / 2) && (moreSteps >= 2))
			return true;
		return false;
	}
}


void _313178576_C::updateSpot(Direction step)
{
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


int _313178576_C::distanceToDocking()
{
	// distance is the distance in rows + distance in columns
	return abs(cell.row - docking.row) + abs(cell.col - docking.col);
}

Direction _313178576_C::oppositeMove(Direction d)
{
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

Direction _313178576_C::goHome(SensorInformation si)
{
	Direction step;
	// reminder: order of precedence: (1) east (2) west (3) south (4) north (5) stay
	if (docking.col > cell.col && si.isWall[0] == false)
		step = Direction::East;
	else if (docking.col < cell.col && si.isWall[1] == false)
		step = Direction::West;
	else if (docking.row > cell.row && si.isWall[2] == false)
		step = Direction::South;
	else if (docking.row < cell.row && si.isWall[3] == false)
		step = Direction::North;
	else
		step = Direction::Stay;
	return step;
}
