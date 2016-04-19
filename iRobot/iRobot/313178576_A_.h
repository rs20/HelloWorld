#ifndef __ALGORITHM_A_H
#define __ALGORITHM_A_H

#include <stdlib.h>
#include <list>

#include "Direction.h"
#include "AbstractAlgorithm.h"

#ifndef __MIN_
#define __MIN
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

/*
First Algorithm A:
1. rememebrs the path to the docking station.
2. does not know where the docking station is, but only how to return to it.
3. hence, is not able to reset path to docking station if entered docking station not on purpose
4. hence, does not know if the battery was charged.
5. logic of steps: - if current spot has dirt in it, then stay
- else, go to direction (in new order precedences: east->south->west->north) that is different than the last move made
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

class _313178576_A : public AbstractAlgorithm
{
private:
	const AbstractSensor* sensor;
	//list<const AbstractSensor*> sensors;
	int moreSteps = -1; // set to unknown until alret from 'aboutToFinish'
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	bool ending = false;
	list<Direction> path; // remember path in order to return to docking station
	int distanceToDocking = 0;
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override;
	virtual void setConfiguration(map<string, int> config) override;
	virtual Direction step() override;
	virtual void aboutToFinish(int stepsTillFinishing) override;

private:
	bool shouldReturnDocking();
	bool isOppositeMove(Direction d1, Direction d2);
	Direction oppositeMove(Direction d);
};

#endif // __ALGORITHM_A_H
