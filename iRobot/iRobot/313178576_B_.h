#ifndef __ALGORITHM_B_H
#define __ALGORITHM_B_H

#include <stdlib.h>
#include <list>

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Cell.h"
#include "AlgorithmRegistration.h"

#ifndef __MIN_
#define __MIN_
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

/*
Second Algorithm B:
1. rememebrs the path to the docking station.
2. remembers where the docking station is, and how to return to it.
3. hence, is able to reset path to docking station if entered docking station once again
4. hence, knows if the battery was charged.
5. logic of steps: - if current spot has dirt in it, then stay
- else, go to the first direction (order of precedence: east->west->south->north) different than the last step made that is available
6. if the move chosen is opposite to the last move made -> remove both from the path (the algorithm is not that dumb)
*/

// Pro comparing to alg. A: knows where the docking station is compared to it's current location at each step


class _313178576_B : public AbstractAlgorithm
{
private:
	//list<const AbstractSensor*> sensors;
	const AbstractSensor* sensor;
	int moreSteps = -1;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;
	bool ending = false;
	//list<Direction> path;
	list<pair<Cell, int>> path; // remember path in order to return to docking station.
	// note that path does not include current position (only one before)
	// path is a pair of cell and distance to docking from that cell
	Direction lastStep = Direction::Stay;
	int distanceToDocking = 0;
	// this is a relative position of the docking since the algorithm does not know the house dimensions / docking station's spot
	Cell docking = Cell();
	Cell cell = Cell();
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	_313178576_B() {};
	_313178576_B(const _313178576_B&) = delete;
	_313178576_B& operator=(const _313178576_B&) = delete;
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override;
	virtual void setConfiguration(map<string, int> config) override;
	virtual Direction step() override;
	virtual void aboutToFinish(int stepsTillFinishing) override;

private:
	bool shouldReturnDocking();
	bool goOnCleaningAgain();
	bool isOppositeMove(Direction d1, Direction d2);
	Direction oppositeMove(Direction d);
	void updateSpot(Direction step);
	// assumes src and dst are exactly one step far from each other
	Direction directionFromCells(Cell src, Cell dst);
};

#endif // __ALGORITHM_B_H