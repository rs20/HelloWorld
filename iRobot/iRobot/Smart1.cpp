//#include "stdafx.h"
#include "Smart1.h"

REGISTER_ALGORITHM(Smart1)


void Smart1::setSensor(const AbstractSensor& s)
{
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	returning = false;
	recharging = false;
	lastMove = Direction::Stay;
	house.resetHouse();
	wayHome.clear();
}

void Smart1::setConfiguration(map<string, int> config)
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

// TODO: add logic for choosing the next step cleverly
Direction Smart1::step(Direction prevStep)
{
	// update robot's position according to the last move that the simulator actually made with the robot
	// as the exercise specifies, the simulator may not work always according to the algorithm's suggested move
	house.updateRobot(prevStep);
	if (prevStep != Direction::Stay)
		lastMove = prevStep;

	// first, if started the move from the docking station -> charge battery
	if (house.getRobot().row == house.getDocking().row && house.getRobot().col == house.getDocking().col)
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	Direction step;
	if (returning) {
		// on the way home -> get the first move in the 'wayHome' list
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty()) {
			returning = false;
			recharging = true;
		}
	}
	else if (recharging && !goClean()) {
		// in docking station but needs to recharge more battery before leaving -> stay
		step = Direction::Stay;
	}
	else if (!returning && !recharging && goHome()) {
		// didn't have to return to the docking station in the last step and is not recharging atm, but now has to return to the docking station
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty())
			recharging = true;
		else
			returning = true;
	}
	else {
		// - recharged enough and should clean OR
		// - is not recharging atm, should not go back to the docking station, and should pick a new step
		recharging = false;
		// (recharging == false, returning == false)
		SensorInformation si = sensor->sense();
		if (si.dirtLevel > 1)
			step = Direction::Stay;
		// choose first step available that is different than the last move made (if possible)
		else {
			int directions = 0;
			// count available moves
			for (int i = 0; i < 4; i++)
				directions += (si.isWall[i]) ? 0 : 1;

			// pick first available direction to move to different than the last step made
			// choose last step only if it is the only available move
			if (directions > 1) {
				if (si.isWall[0] == false && oppositeMove(lastMove) != Direction::East)
					step = Direction::East;
				else if (si.isWall[1] == false && oppositeMove(lastMove) != Direction::West)
					step = Direction::West;
				else if (si.isWall[2] == false && oppositeMove(lastMove) != Direction::South)
					step = Direction::South;
				else if (si.isWall[3] == false && oppositeMove(lastMove) != Direction::North)
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

	// consume battery only if did not start the move from the docking station
	Cell robot = house.getRobot();
	Cell docking = house.getDocking();
	if (robot.row != docking.row || robot.col != docking.col)
		curBattery -= batteryConsumptionRate;
	
	// do not update spot according to 'step' chosen because simulator may not choose the algorithm's suggested step!
	// updated only at start of this method (sense) -> the simulator passes the actual step that has been made

	if (moreSteps != -1)
		moreSteps--;
	return step;
}

void Smart1::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}

Direction Smart1::oppositeMove(Direction d)
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

// returns true <-> should start going back to the docking station
// if true -> 1. ending flag will be true (ending = true)
//			  2. 'wayHome' list should be updated with directions list(!!)
//					so the algorithm wil be able to use the list on the its way home
//					(without calling goHome on the way home)
bool Smart1::goHome()
{
	if (house.getRobot() == house.getDocking())
		return false;
	// BFS (shortest path to the docking station) is implemented in MyHouse-toDocking and it returns list<Direction>
	list<Direction> path = std::move(house.toDocking());
	int distanceToDocking = path.size();
	int movesToMake = curBattery / batteryConsumptionRate; // (1.9 -> 1)
	// if moreSteps is up to date -> take into consideration
	if (moreSteps != -1)
		movesToMake = MIN(movesToMake, moreSteps);
	if (distanceToDocking >= movesToMake - 2) // if so, wayHome is updated and contains the path to the docking station
		return true;
	return false;
}

bool Smart1::goClean()
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