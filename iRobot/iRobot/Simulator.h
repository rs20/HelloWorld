
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "Auxiliary.h"
#include "Score.h"
#include "Sensor.h"

class Simulator {
public:
	void startSimulation(House* houses, int numOfHouses, int numOfAlgorithms, map<string, int> config, S_Algorithm* algorithms);
};