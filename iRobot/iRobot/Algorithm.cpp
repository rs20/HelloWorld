#include "stdafx.h"

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Sensor.cpp"

#include <time.h>
#include <vector>


class Algorithm : public AbstractAlgorithm
{
private:
	Sensor sensor;
	int numOfSteps = 0;
	int moreSteps;
	int batteryCapacity;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	// note that the algorithm does not have an access to the house.
public:
	Algorithm() {}
	void setSensor(const AbstractSensor& s) {
		sensor = s;
	}
	void setConfiguration(map<string, int> config) {
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
	Direction step() {
		SensorInformation si = sensor.sense();
		int numOfOptions = 1; // may always stay in the same place
		vector<Direction> vec;
		vec.push_back(Direction::Stay);
		for (int i = 0; i < 3; i++)
			if (si.isWall[i] == true) {
				numOfOptions++;
				vec.push_back(Direction(i));
			}
		// initialize random seed
		srand((unsigned int)time(NULL));
		// generate random number between 0 and numOfOptions-1
		int randomStep = rand() % numOfOptions;
		return vec.at(randomStep);
	}
	void aboutToFinish(int stepsTillFinishing) {
		moreSteps = stepsTillFinishing;
	}

	void madeStep() {
		numOfSteps++;
		moreSteps--;
		batteryCapacity -= batteryConsumptionRate;
	}
	int getBatteryCapacity() {
		return batteryCapacity;
	}
	int getMovesAvailable() {
		return moreSteps;
	}
};