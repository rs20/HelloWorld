#include "SuperAlgorithm.h"


void SuperAlgorithm::setSensor(const AbstractSensor& s)
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

void SuperAlgorithm::setConfiguration(map<string, int> config)
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

void SuperAlgorithm::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}

Direction SuperAlgorithm::oppositeMove(Direction d)
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
bool SuperAlgorithm::goHome()
{
	if (house.getRobot() == house.getDocking())
		return false;
	// BFS (shortest path to the docking station) is implemented in MyHouse-toDocking and it returns list<Direction>
	Cell homeCell;
	wayHome = std::move(house.BFS(house.getRobot(), homeCell, 'D'));
	int distanceToDocking = wayHome.size();
	int movesToMake = curBattery / batteryConsumptionRate; // (1.9 -> 1)
														   // if moreSteps is up to date -> take into consideration
	if (moreSteps != -1)
		movesToMake = MIN(movesToMake, moreSteps);
	if (distanceToDocking >= movesToMake - 2) // if so, wayHome is updated and contains the path to the docking station
		return true;
	return false;
}

bool SuperAlgorithm::goClean()
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