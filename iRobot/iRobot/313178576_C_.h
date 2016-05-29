#ifndef __ALGORITHM_C_H
#define __ALGORITHM_C_H
#include "SuperAlgorithm.h"

/*
Smart Algorithm 2:
1. remembers where the docking station is.
2. hence, is able to reset path to docking station if entered docking station once again
3. hence, knows if the battery was charged.
4. while cleaning, builds his own view of the house.
5. in each step; runs a BFS on the house (can be viewed as a graph) to check distance from docking, and then checks if should return home
6. logic of steps:	- if current spot has dirt in it, then stay
					- else, if next to a wall, continue cleaning along the wall
					- else, go to the closest (BFS) cell of 'X' (not visited yet and is not a wall)
7. recharges until full battery when back to docking station
8. preference order of steps (left to right): west, east, south, north, stay
*/

class _313178576_C : public SuperAlgorithm
{
public:
	// block improperly handled constructors
	_313178576_C() {};
	_313178576_C(const _313178576_C&) = delete;
	_313178576_C& operator=(const _313178576_C&) = delete;
	virtual Direction step(Direction prevStep) override;
};
#endif
