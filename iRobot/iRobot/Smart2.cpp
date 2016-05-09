#include "Smart2.h"

REGISTER_ALGORITHM(Smart2)


void Smart2::setSensor(const AbstractSensor& s)
{
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	returning = false;
	recharging = false;
	goingX = false;
	lastMove = Direction::Stay;
	house.resetHouse();
	wayHome.clear();
	xPath.clear();
	end = false;
}

void Smart2::setConfiguration(map<string, int> config)
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

Direction Smart2::step(Direction prevStep)
{
	// update robot's position according to the last move that the simulator actually made with the robot
	// as the exercise specifies, the simulator may not work always according to the algorithm's suggested move
	house.updateRobot(prevStep);

	// check if the simulator took the algorithm's advice with the last steps
	if (prevStep != lastMove) {
		// reset all! algorithm will recalculate destination
		returning = false;
		recharging = false;
		goingX = false;
		end = false;
		wayHome.clear();
		xPath.clear();
	}
	// update
	if (prevStep != Direction::Stay)
		lastMove = prevStep;

	// first, if started the move from the docking station -> charge battery
	if (house.getRobot() == house.getDocking())
		curBattery += batteryRechargeRate;
	curBattery = MIN(curBattery, batteryCapacity);

	// update area
	SensorInformation si = sensor->sense();
	house.updateRobotArea(si);

	Direction step;

	// cleaned the whole house -> go to the docking station and stay there
	if (end) {
		if (wayHome.empty())
			step = Direction::Stay;
		else {
			step = wayHome.front();
			wayHome.pop_front();
		}
	}
	// on the way home -> get the first move in the 'wayHome' list
	else if (returning) {
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty()) {
			returning = false;
			recharging = true;
		}
	}
	// in docking station but needs to recharge more battery before leaving -> stay
	else if (recharging && !goClean()) {
		step = Direction::Stay;
	}
	// didn't have to return to the docking station in the last step and is not recharging atm, but now has to return to the docking station
	else if (!returning && !recharging && goHome()) {
		goingX = false;
		step = wayHome.front();
		wayHome.pop_front();
		if (wayHome.empty())
			recharging = true;
		else
			returning = true;
	}
	// on the way to a new cell with 'X'
	else if (goingX) {
		step = xPath.front();
		xPath.pop_front();
		if (xPath.empty())
			goingX = false;
	}
	// - recharged enough and should clean OR
	// - is not recharging atm, should not go back to the docking station, and should pick a new step
	else {
		recharging = false;
		// (recharging == false, returning == false)
		// clean all dirt, then move to a new cell
		if (si.dirtLevel > 1)
			step = Direction::Stay;
		else {
			// get the closest cell with 'X' and move to it
			Cell cell = house.getRobot();
			Cell east = { cell.row, cell.col + 1 };
			Cell west = { cell.row, cell.col - 1 };
			Cell south = { cell.row + 1, cell.col };
			Cell north = { cell.row - 1, cell.col };
			// first clean immediate dirt around you
			if (house.hasCell(west) && (house.getCell(west) > '0') && (house.getCell(west) <= '9'))
				step = Direction::West;
			else if (house.hasCell(east) && (house.getCell(east) > '0') && (house.getCell(east) <= '9'))
				step = Direction::East;
			else if (house.hasCell(south) && (house.getCell(south) > '0') && (house.getCell(south) <= '9'))
				step = Direction::South;
			else if (house.hasCell(north) && (house.getCell(north) > '0') && (house.getCell(north) <= '9'))
				step = Direction::North;
			// then go to immediate X around you
			else if (house.hasCell(west) && (house.getCell(west) == 'X'))
				step = Direction::West;
			else if (house.hasCell(east) && (house.getCell(east) == 'X'))
				step = Direction::East;
			else if (house.hasCell(south) && (house.getCell(south) == 'X'))
				step = Direction::South;
			else if (house.hasCell(north) && (house.getCell(north) == 'X'))
				step = Direction::North;
			// then search for the closest X / dirt
			else {
				// no immediate 'X' cells around robot
				// run BFS to get 'path' updated to hold shortest path to the closest 'X' or '1'-'9' in the house
				// note that we may find '1'-'9' cells (actually 9 is not possible) in case we had to go back
				// to the docking station while cleaning a cell
				xPath = std::move(house.BFS('X'));
				if (xPath.empty()) {
					end = true;
					// no 'X' was found -> cleaned the whole house! go Home!
					// BFS (shortest path to the docking station)
					wayHome = std::move(house.BFS('D'));
					step = wayHome.front();
					wayHome.pop_front();
				}
				else {
					step = xPath.front();
					xPath.pop_front();
					if (!xPath.empty())
						goingX = true;
				}
			}
		}
	}

	// consume battery only if did not start the move from the docking station
	if (house.getRobot() != house.getDocking())
		curBattery -= batteryConsumptionRate;

	// do not update spot according to 'step' chosen because simulator may not choose the algorithm's suggested step!
	// updated only at start of this method (sense) -> the simulator passes the actual step that has been made
	lastMove = step;

	if (moreSteps != -1)
		moreSteps--;
	return step;
}

void Smart2::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}

Direction Smart2::oppositeMove(Direction d)
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
bool Smart2::goHome()
{
	if (house.getRobot() == house.getDocking())
		return false;
	// BFS (shortest path to the docking station) is implemented in MyHouse-toDocking and it returns list<Direction>
	wayHome = std::move(house.BFS('D'));
	int distanceToDocking = wayHome.size();
	int movesToMake = curBattery / batteryConsumptionRate; // (1.9 -> 1)
														   // if moreSteps is up to date -> take into consideration
	if (moreSteps != -1)
		movesToMake = MIN(movesToMake, moreSteps);
	if (distanceToDocking >= movesToMake - 2) // if so, wayHome is updated and contains the path to the docking station
		return true;
	return false;
}

bool Smart2::goClean()
{
	// don't know how many more moves yet -> check battery recharged enough
	if (moreSteps == -1) {
		// charged enough
		if (curBattery == batteryCapacity)
			return true;
		return false;
	}
	// check both moreSteps and curBattery and decide according to both
	else {
		if ((curBattery == batteryCapacity) && (moreSteps >= 2))
			return true;
		return false;
	}
}