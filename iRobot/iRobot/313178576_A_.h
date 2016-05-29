#ifndef __ALGORITHM_A_H
#define __ALGORITHM_A_H
#include "SuperAlgorithm.h"

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
8. preference order of steps (left to right): east, west, south, north, stay
*/

class _313178576_A : public SuperAlgorithm
{
public:
	// block improperly handled constructors
	_313178576_A() {};
	_313178576_A(const _313178576_A&) = delete;
	_313178576_A& operator=(const _313178576_A&) = delete;
	virtual Direction step(Direction prevStep) override;
};
#endif
