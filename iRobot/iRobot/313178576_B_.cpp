//#include "stdafx.h"

#include <stdlib.h>
#include <list>
#include "313178576_B_.h"

/*
Second Algorithm B:
1. rememebrs the path to the docking station.
2. remembers where the docking station is, and how to return to it.
3. hence, is able to reset path to docking station if entered docking station once again
4. hence, knows if the battery was charged.
5. logic of steps: - if current spot has dirt in it, then stay
- else, go to the first direction (order of precedence: east->west->south->north) different than the last step made that is available
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

// Pro comparing to alg. A: knows where the docking station is compared to it's current location at each step


void _313178576_B::setSensor(const AbstractSensor& s)
{
	//sensors.push_back(&s);
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	ending = false;
	path.clear();
	distanceToDocking = 0;
	cell = { 0, 0 };
	docking = { 0, 0 };
	lastStep = Direction::Stay;
}

void _313178576_B::setConfiguration(map<string, int> config)
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

Direction _313178576_B::step()
{
	// first, if started the move from the docking station -> charge battery
	if (cell.row == docking.row && cell.col == docking.col)
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	Direction step;
	// go back to docking station - get the last move made
	if (ending && !path.empty()) {
		step = directionFromCells(cell, path.back().first);
		path.pop_back();
		distanceToDocking--;
	}
	else if (ending && path.empty() && !goOnCleaningAgain()) {
		// reached docking, waiting for end of game/recharge
		step = Direction::Stay;
		//distanceToDocking==0
	}
	else if (!ending && shouldReturnDocking()) {
		ending = true;
		// reached docking, waiting for end of game
		if (path.empty()) {
			step = Direction::Stay;
			// distanceToDocking==0
			distanceToDocking = 0;
		}
		// get the last move made
		else {
			step = directionFromCells(cell, path.back().first);
			path.pop_back();
			distanceToDocking--;
		}
	}
	// continue cleaning OR reaches here if already back in docking station but has enough battery to continue cleaning again
	else {
		//SensorInformation si = sensors.back()->sense();
		ending = false;
		SensorInformation si = sensor->sense();

		// move into cell without staying does clean the spot anyway
		if (si.dirtLevel > 1)
			step = Direction::Stay;
		else {
			int directions = 0;
			// count available moves
			for (int i = 0; i < 4; i++)
				directions += (si.isWall[i]) ? 0 : 1;

			// if more than one move is available -> do not repeat last move made

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

			// if moved (!= stay) -> add move to path and increment distance to docking
			if (step != Direction::Stay) {
				lastStep = step;
				// if step is the opposite of the last move -> remove both moves (2 last moves) from path list (returned to last cell)
				if (!path.empty() && cell.row == path.back().first.row && cell.col == path.back().first.col) {
					path.pop_back();
					distanceToDocking--;
				}
				else {
					path.emplace_back(cell, distanceToDocking); // insert current position (before move) to path list
					distanceToDocking++;
				}
			}
		}
	}

	// consume battery only if did not start the move from the docking station
	if (cell.row != docking.row || cell.col != docking.col)
		curBattery -= batteryConsumptionRate;

	updateSpot(step);
	// if returned to docking station: empty path and distance to docking
	if (cell.row == docking.row && cell.col == docking.col) {
		path.clear();
		distanceToDocking = 0;
	}
	// if visits some spot it already has visited! remove all cells (including the moved to cell) from first time visited the cell
	for (list<pair<Cell, int>>::iterator it = path.begin(); it != path.end(); it++) {
		// if reaches some old visited location -> delete all cells starting from that cell
		if (it->first.row == cell.row && it->first.col == cell.col) {
			while (it != path.end())
				path.erase(it++);

			if (path.empty())
				distanceToDocking = 0;
			else
				distanceToDocking = path.back().second; // get distance to docking from the last cell
			break;
		}
	}
	if (moreSteps != -1)
		moreSteps--;
	return step;
}

void _313178576_B::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}

bool _313178576_B::shouldReturnDocking()
{
	int movesToMake = curBattery / batteryConsumptionRate; // 1.9 -> 1
	// if more steps is up to date -> take into consideration
	if (moreSteps != -1)
		movesToMake = MIN(movesToMake, moreSteps);
	// ___
	//|_|_|
	//|D|R|
	// robot distance to docking is 1, but has 2 more moves to make
	// if it will go north -> he won't be able to return to the docking station
	// its '-2' because we don't want to return to the docking station exactly with empty battery -> this means we DIE.
	// so go back with extra for one move which means you will be able to continue after recharging yourself
	if (distanceToDocking >= movesToMake - 2)
		return true;
	return false;
}

bool _313178576_B::goOnCleaningAgain()
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

bool _313178576_B::isOppositeMove(Direction d1, Direction d2)
{
	if (((d1 == Direction::East) && (d2 == Direction::West)) ||
		((d1 == Direction::West) && (d2 == Direction::East)) ||
		((d1 == Direction::South) && (d2 == Direction::North)) ||
		((d1 == Direction::North) && (d2 == Direction::South)))
		return true;
	return false;
}

Direction _313178576_B::oppositeMove(Direction d)
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


void _313178576_B::updateSpot(Direction step)
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

// assumes src and dst are exactly one step far from each other
Direction _313178576_B::directionFromCells(Cell src, Cell dst)
{
	if (src.row == dst.row - 1)
		return Direction::South;
	else if (src.row == dst.row + 1)
		return Direction::North;
	else if (src.col == dst.col - 1)
		return Direction::East;
	else
		return Direction::West;
}

#ifdef __linux__ 	
extern "C" AbstractAlgorithm* maker()
{
	return new _313178576_B();
}
#endif