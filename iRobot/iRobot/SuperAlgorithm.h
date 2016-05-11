#include <stdlib.h>

#include "MyHouse.h"
#include "AbstractAlgorithm.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"

#ifndef __MIN_
#define __MIN_
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

/*
base class for all algorithms (inherits from AbstractAlgorithm)
*/

class SuperAlgorithm : public AbstractAlgorithm
{
protected:
	const AbstractSensor* sensor;
	int moreSteps = -1; // steps until simulation's finishes / -1 if unknown
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	MyHouse house;
public:
	virtual void setConfiguration(map<string, int> config) override;
	virtual void aboutToFinish(int stepsTillFinishing) override;
	Direction oppositeMove(Direction d);
};