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
- else, go to the closest (BFS) cell of 'X' (not visited yet and is not a wall)
7. recharges until full battery when back to docking station
8. preference order of steps (left to right): south, north, east, west, stay
*/

class Smart3 : public AbstractAlgorithm
{
private:
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
	list<Direction> xPath; // used for going to the closest x in the shortest number of steps
	bool end = false; // when cleaned the whole house
public:
	// block improperly handled constructors
	Smart3() {};
	Smart3(const Smart3&) = delete;
	Smart3& operator=(const Smart3&) = delete;
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