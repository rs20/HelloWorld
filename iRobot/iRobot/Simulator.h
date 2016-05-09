
#include "Auxiliary.h"
#include "Score.h"
#include "Sensor.h"

class Simulator {
	int numOfHouses;
	int numOfAlgorithms;
	int numOfThreads;
	bool score_loaded = false;
	void* score_hndl = NULL;
	int (*score_function)(const map<string, int>& score_params);
	unique_ptr<House[]> houses;
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	std::map<string, int> config = {};
	vector<string> flags{ defaultConfigPath, defaultScorePath, defaultHousePath, defaultAlgorithmPath, defaultThreads };
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