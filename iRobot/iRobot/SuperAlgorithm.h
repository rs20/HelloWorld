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
	Direction lastMove = Direction::Stay; // remember last move (that is different than Stay)
	bool returning = false; // true <-> on the way to the docking station
	bool recharging = false; // true <-> need to recharge battery before cleaning again
	bool goingX = false; // true <-> on the way to a new place with 'X'
	list<Direction> wayHome; // after calling goHome -> it is updated and holds tha shortest path to the docking station
	list<Direction> xPath; // used for going to the x
	bool end = false; // when cleaned the whole house
public:
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override;
	virtual void setConfiguration(map<string, int> config) override;
	virtual void aboutToFinish(int stepsTillFinishing) override;
protected:
	Direction oppositeMove(Direction d);
	bool goHome();
	virtual bool goClean();
};