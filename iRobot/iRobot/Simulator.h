
#include "Auxiliary.h"
#include "Score.h"
#include "Sensor.h"

class Simulator {
	int numOfHouses;
	int numOfAlgorithms;
	House* houses = nullptr;
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	std::map<string, int> config = {};
	// vector of length 3: [0] holds config path, [1] holds house path and [2] holds algorithm path
	// if not specified, place default
	vector<string> flags{ defaultConfigPath, defaultHousePath, defaultAlgorithmPath };
public:
	void handleArguments(int argc, const char* argv[]);
	int handleConfiguration();
	int handleAlgorithms();
	int handleHouses();
	//void startSimulation(House* houses, int numOfHouses, int numOfAlgorithms, map<string, int> config, AlgorithmRegistrar &algorithms);
	void startSimulation();
	void end();
};