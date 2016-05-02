//#include "stdafx.h"
#include "Smart1.h"

REGISTER_ALGORITHM(Smart1)


void Smart1::setSensor(const AbstractSensor& s)
{
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	ending = false;
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

	// first, if started the move from the docking station -> charge battery
	if (house.getRobot().row == house.getDocking().row && house.getRobot().col == house.getDocking().col)
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	Direction step;
	if (ending) {
		// on the way home -> get the first move in the 'wayHome' list
		if (wayHome.empty() == 0)
			step = Direction::Stay;
		else {
			step = wayHome.back();
			wayHome.pop_back();
		}
	}
	// check if should go back to docking station (and turn on ending flag + put way of directions to the docking stations in wayHome list)
	//else if (should go back docking station)
	else {
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
				if (si.isWall[0] == false && oppositeMove(prevStep) != Direction::East)
					step = Direction::East;
				else if (si.isWall[1] == false && oppositeMove(prevStep) != Direction::West)
					step = Direction::West;
				else if (si.isWall[2] == false && oppositeMove(prevStep) != Direction::South)
					step = Direction::South;
				else if (si.isWall[3] == false && oppositeMove(prevStep) != Direction::North)
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

void Smart1::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}

// returns true <-> should start retracking to the docking station
// if true -> 1. ending flag will be true (ending = true)
//			  2. 'wayHome' list should be updated with directions(!!) so the algorithm wil be able to pop the last direction and commit
//					(without calling goHome on the way home)
bool Smart1::goHome()
{
	return false;
	// TODO: add BFS search on the house matrix to find out distance to docking station and find the shortest way

	// notice the next example when deciding whether the robot should head back home
	// | |_| |
	// |D|_|R|
	// the distance is 2, suppose the robot has 3 more moves to make: - if he chooses not to go home (go north -> he's screwed - not enough to go home)
	// suppose the robot battery is 3 (-1 each step) and he chooses to go north... same idea
}