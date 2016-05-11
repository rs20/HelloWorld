#include "SuperAlgorithm.h"

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