// Algorithm.cpp
#include "stdafx.h"

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Sensor.cpp"

#include <time.h>
#include <vector>
#include <iostream>


class Algorithm : public AbstractAlgorithm
{
private:
	const AbstractSensor* sensor;
	int moreSteps;
	int batteryCapacity;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	virtual void setSensor(const AbstractSensor& s) override {
		sensor = &s;
	}
	virtual void setConfiguration(map<string, int> config) override {
		map<string, int>::iterator it;
		it = config.find("MaxSteps");
		moreSteps = it->second;
		it = config.find("BatteryCapacity");
		batteryCapacity = it->second;
		it = config.find("BatteryConsumptionRate");
		batteryConsumptionRate = it->second;
		it = config.find("BatteryRechargeRate");
		batteryRechargeRate = it->second;
	}
	// implement naive selection of direction to move to using the sensor
	virtual Direction step() override {
		SensorInformation si = sensor->sense();
		int numOfOptions = 1; // may always stay in the same place
		vector<Direction> vec;
		vec.push_back(Direction::Stay);
		for (int i = 0; i < 4; i++)
			if (si.isWall[i] == false) {
				numOfOptions++;
				vec.push_back(Direction(i));
			}
		// initialize random seed
		srand((unsigned int)time(NULL));
		// generate random number between 0 and numOfOptions-1
		int randomStep = rand() % numOfOptions;
		return vec.at(randomStep);
	}
	virtual void aboutToFinish(int stepsTillFinishing) override {
		moreSteps = stepsTillFinishing;
	}
};