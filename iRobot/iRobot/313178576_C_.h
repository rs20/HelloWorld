#ifndef __ALGORITHM_C_H
#define __ALGORITHM_C_H

#include <stdlib.h>
#include <list>

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Cell.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"

#ifndef __MIN_
#define __MIN_
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

/*
Third Algorithm C:
1. adventurous algorithm: will not return to the docking station in the same order it left it.
2. it just remembers the relative location of the docking station and the relative spot of the robot, and tries to return to it in a straight line.
3. if the algorithm wants to return to the docking station, say the direction of the docking station relative to current cell is nort west,
the algorithm will make only north/west steps.
according to the precedence in Direction.h (west < north); it will try making all the west steps,
and only after that (stuck / got to the same column as the docking station) it will make north steps
and if stuck again, back to west steps and so on until reaches docking station
if stuck on both direction, just stay at the same spot
4. logic of steps: - if current spot has dirt in it, then stay
- else, go to the first direction (order of precedence: east->west->south->north) different than the last step made that is available
*/

// Pro: may be more efficient
// Con: probably will not return to the docking station successfully


class _313178576_C : public AbstractAlgorithm
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
	// this is a relative position of the docking since the algorithm does not know the house dimensions / docking station's spot
	Cell docking;
	Cell cell;
	// remember last step
	Direction lastStep = Direction::Stay;
	// note that the algorithm does not have an access to the house (only its sensor).
public:
	_313178576_C() {};
	_313178576_C(const _313178576_C&) = delete;
	_313178576_C& operator=(const _313178576_C&) = delete;
	// set new sensor -> algorithm knows: starting to work on a new house
	virtual void setSensor(const AbstractSensor& s) override;
	virtual void setConfiguration(map<string, int> config) override;
	virtual Direction step(Direction prevStep) override;
	virtual void aboutToFinish(int stepsTillFinishing) override;

private:
	bool shouldReturnDocking();
	bool goOnCleaningAgain();
	void updateSpot(Direction step);
	int distanceToDocking();
	Direction oppositeMove(Direction d);
	Direction goHome(SensorInformation si);
};

#endif // __ALGORITHM_C_H
