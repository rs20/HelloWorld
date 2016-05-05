#ifndef __SMART1_H
#define __SMART1_H

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
Smart Algorithm 1:
1. remembers where the docking station is.
2. hence, is able to reset path to docking station if entered docking station once again
3. hence, knows if the battery was charged.
4. while cleaning, builds his own view of the house.
5. in each step; runs a BFS on the house (can be viewed as a graph) to check distance from docking, and then checks if should return home
6. logic of steps:	- if current spot has dirt in it, then stay
					- else, go to the first direction (order of precedence: east->west->south->north) different than the last step made that is available
*/

class Smart1 : public AbstractAlgorithm
{
private:
	bool start = true;
	const AbstractSensor* sensor;
	int moreSteps = -1; // steps until simulation's finishes / -1 if unknown
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	MyHouse house;
	Direction lastMove = Direction::Stay; // remember last move different than Stay
	bool returning = false; // true <-> on the way to the docking station
	bool recharging = false; // true <-> need to recharge battery before cleaning again
	list<Direction> wayHome;
public:
	// block improperly handled constructors
	Smart1() {};
	Smart1(const Smart1&) = delete;
	Smart1& operator=(const Smart1&) = delete;
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override;
	virtual void setConfiguration(map<string, int> config) override;
	virtual Direction step(Direction prevStep) override;
	virtual void aboutToFinish(int stepsTillFinishing) override;
private:
	Direction oppositeMove(Direction d);
	bool goHome();
	bool goClean();
};

#endif // __SMART1_H