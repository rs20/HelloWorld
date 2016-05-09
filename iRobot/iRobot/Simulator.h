
#include "Auxiliary.h"
#include "Score.h"
#include "Sensor.h"

class Simulator {
	int numOfHouses;
	int numOfAlgorithms;
	int numOfThreads;
	bool score_loaded = false;
	void* score_hndl;
	unique_ptr<House[]> houses;
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	std::map<string, int> config = {};
	vector<string> flags{ defaultConfigPath, defaultScorePath, defaultHousePath, defaultAlgorithmPath };
public:
	void handleArguments(int argc, const char* argv[]);
	int handleConfiguration();
	int handleScore();
	int handleAlgorithms();
	int handleHouses();
	int handleThreads();
	void startSimulation();
	void end();
};