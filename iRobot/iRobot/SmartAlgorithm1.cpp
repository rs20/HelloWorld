//#include "stdafx.h"
#include "SmartAlgorithm1.h"

REGISTER_ALGORITHM(SmartAlgorithm1)


void SmartAlgorithm1::setSensor(const AbstractSensor& s)
{
	sensor = &s;
	moreSteps = -1;
	curBattery = batteryCapacity;
	ending = false;
	lastStep = Direction::Stay;
}

void SmartAlgorithm1::setConfiguration(map<string, int> config)
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

Direction SmartAlgorithm1::step(Direction prevStep)
{
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
	else {

	}
	return step;
}

void SmartAlgorithm1::aboutToFinish(int stepsTillFinishing)
{
	moreSteps = stepsTillFinishing;
}