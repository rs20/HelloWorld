// Simulation (main,cpp) : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <string>
#include <vector>
#include "Auxiliary.h"
#include "Simulator.h"
#include "Score.h"
#include "Sensor.cpp"
#include <iomanip>

#define DEBUG 0

// assumes all algorithms that reach here are fine
void startSimulation(House* houses, int numOfHouses, int numOfAlgorithms, map<string, int> config)
{
	// matrix (vector of vectors) of scores: scores[0] - scores of the first house on every algorithm and so on
	vector<vector<int>> scores(numOfHouses, vector<int>(numOfAlgorithms));
	bool is_back_in_docking;
	//vector<AbstractAlgorithm> algorithms(numOfAlgorithms); // TODO: this should be in the signature of the function (main sends it)
	AbstractAlgorithm** algorithms = new AbstractAlgorithm*[numOfAlgorithms];
	vector<string> algorithmsNames;

	algorithmsNames.push_back("313178576_A_");
	algorithmsNames.push_back("313178576_B_");
	algorithmsNames.push_back("313178576_C_");

	// specific to house
	int max_steps;
	int simulation_num_steps;
	int batteryCapacity;
	int batteryConsumptionRate;
	int batteryRechargeRate;

	int numOfWorkingHouses = 0;

	House* curHouses;

	// iterate over all houses
	for (int k = 0; k < numOfHouses; k++)
	{
		if (!(houses[k].isValidHouse))
			continue;
		numOfWorkingHouses++;

		// just checking to see if it worked (print the house) - for debug purpose
		if (houses[k].matrix != NULL) {
			for (int i = 0; i < houses[k].rows; i++) {
				for (int j = 0; j < houses[k].cols; j++) {
					cout << houses[k].matrix[i][j] << " ";
				}
				cout << endl;
			}
		}

		// start the game:
		vector<bool> if_end(numOfAlgorithms);
		vector<bool> into_wall(numOfAlgorithms);
		vector<int> curBattery(numOfAlgorithms);
		vector<int> numSteps(numOfAlgorithms);
		vector<int> positionInComp(numOfAlgorithms, 10); // default position is 10
		bool is_winner = false;
		int winner_num_steps = -1;
		int cur_stage_winners = 0;
		int cur_position = 1;
		int finished = 0;

		// automatic win for each algorithm if the dirt in the house == 0
		if (houses[k].sumOfDirt == 0) {
			for (int i = 0; i < numOfAlgorithms; i++)
				scores[k][i] = MAX_SCORE;
			continue; // to next house
		}

		// make a copy of the current house for every algorithm and assign a sensor to it
		curHouses = new House[numOfAlgorithms];
		vector<Sensor> sensors;
		//vector<Sensor*> sensors(numOfAlgorithms);
		for (int l = 0; l < numOfAlgorithms; l++) {
			copyHouse(curHouses[l], houses[k]);
			sensors.emplace_back(Sensor(&curHouses[l]));
			//sensors[l](&curHouses[l]);
			//algorithms[l].setSensor(*(sensors[l]));
			//algorithms[l]->setSensor(sensors[l]);
			//algorithms[l]->setConfiguration(config);
		}



		// CHOOSE YOUR ALGORITHM
		/*
		_313178576_A alg_a;
		alg_a.setSensor(sensors[0]);
		alg_a.setConfiguration(config);
		algorithms[0] = &alg_a;
		*/
		
		_313178576_B alg_b;
		alg_b.setSensor(sensors[0]);
		alg_b.setConfiguration(config);
		algorithms[0] = &alg_b;
		
		/*
		_313178576_C alg_c;
		alg_c.setSensor(sensors[0]);
		alg_c.setConfiguration(config);
		algorithms[0] = &alg_c;
		*/

		max_steps = houses[k].maxSteps;
		batteryCapacity = (config.find("BatteryCapacity"))->second;
		for (int i = 0; i < numOfAlgorithms; i++)
			curBattery[i] = batteryCapacity;
		batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;
		batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;
		simulation_num_steps = 0;

		cout << endl;
		printHouseWithRobot(houses[k]);
		
		while (true) {
			simulation_num_steps++;
			//Sleep(3000);
			getchar();
			// simulate one step for each algorithm
			for (int l = 0; l < numOfAlgorithms; l++) {
				if (if_end[l] == true)
					continue;

				Direction direction = algorithms[l]->step();

				//cleaning dust if there is any.
				if (curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] > '0' && curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] <= '9') {
					curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] = curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] - 1;
					curHouses[l].sumOfDirt--;
				}

				if (curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] == 'D') {
					curBattery[l] = MIN(batteryCapacity, curBattery[l] + batteryRechargeRate);
				}


				// consume battery only if did not start the move from the docking station
				// staying or starting the move from the docking station does not consume battery
				if (curHouses[l].robot.row != curHouses[l].docking.row || curHouses[l].robot.col != curHouses[l].docking.col)
					curBattery[l] -= batteryConsumptionRate;

				switch (direction)
				{
				case static_cast<Direction>(0) :
					curHouses[l].robot.col++;
					break;
				case static_cast<Direction>(1) :
					curHouses[l].robot.col--;
					break;
				case static_cast<Direction>(2) :
					curHouses[l].robot.row++;
					break;
				case static_cast<Direction>(3) :
					curHouses[l].robot.row--;
					break;
				default:
					break;
					// do nothing for 'Stay'
				}

				if (curHouses[l].matrix[curHouses[l].robot.row][curHouses[l].robot.col] == 'W') { // walked into a wall -> stop the algorithm immediately. its score will be zero
					into_wall[l] = true;
					if_end[l] = true;
					finished++;
					if (DEBUG)
						cout << INTO_WALL << endl;
					continue;
				}

				// for debug purpose
				cout << "Step " << simulation_num_steps << endl;
				cout << "Robot Battery: " << curBattery[l] << endl;
				printHouseWithRobot(curHouses[l]);

				if (curHouses[l].sumOfDirt == 0 && curHouses[l].robot.row == curHouses[l].docking.row && curHouses[l].robot.col == curHouses[l].docking.col) {
					if (DEBUG)
						cout << "Robot wins (cleaned the whole house in the limited time)." << endl; //  for debug purpose
					if_end[l] = true;
					cur_stage_winners++;
					if (!is_winner) {
						is_winner = true;
						winner_num_steps = simulation_num_steps;
						max_steps = MIN(max_steps, simulation_num_steps + config["MaxStepsAfterWinner"]);
					}
					finished++;
					positionInComp[l] = cur_position;
					numSteps[l] = simulation_num_steps;
					continue;
				}
				if (curBattery[l] <= 0) {
					if (DEBUG)
						cout << BATTERY_DEAD << endl; // for debug purpose
					if_end[l] = true;
					finished++;
					continue;
				}
			}
			if (cur_stage_winners > 0)
				cur_position = MIN(4, cur_position + cur_stage_winners);
			cur_stage_winners = 0;
			if (finished == numOfAlgorithms || simulation_num_steps == max_steps) {
				if (DEBUG)
					cout << NO_MORE_MOVES << endl;  // for debug purpose
				for (int l = 0; l < numOfAlgorithms; l++) {
					// if didn't change -> he didn't win. set number of steps to simulation steps
					if (numSteps[l] == 0) {
						numSteps[l] = simulation_num_steps;
					}
				}
				break;
			}
		}
		
		// score the algorithms on the house
		// if none won -> winner num steps = simulation num steps
		if (winner_num_steps == -1)
			winner_num_steps = simulation_num_steps;
		for (int l = 0; l < numOfAlgorithms; l++) {
			is_back_in_docking = (curHouses[l].robot.row == curHouses[l].docking.row && curHouses[l].robot.col == curHouses[l].docking.col) ? true : false;
			if (into_wall[l] == true) // if walked into a wall, score=0
				scores[k][l] = 0;
			else if (houses[k].sumOfDirt == 0 && is_back_in_docking)
				scores[k][l] = score(positionInComp[l], winner_num_steps, numSteps[l], curHouses[l].initialSumOfDirt - curHouses[l].sumOfDirt, curHouses[l].initialSumOfDirt, is_back_in_docking);
			else
				scores[k][l] = score(10, winner_num_steps, numSteps[l], curHouses[l].initialSumOfDirt - curHouses[l].sumOfDirt, curHouses[l].initialSumOfDirt, is_back_in_docking);
		}
		getchar();

		// delete cur houses and sensors
		for (int l = 0; l < numOfHouses; l++) {
			for (int i = 0; i < curHouses[l].rows; i++)
				delete[] curHouses[l].matrix[i];
			delete[] curHouses[l].matrix;
			//delete sensors[l];
		}
		delete[] curHouses;
	}

	// print scores
	if (numOfHouses > 0) {
		// print first row
		int dashes = 15 + 11 * (numOfWorkingHouses + 1);
		cout << string(dashes, '-') << endl;
		cout << "|" << string(13, ' ') << "|";
		for (int i = 0; i < numOfHouses; i++) {
			if (houses[i].isValidHouse) {
				int index = houses[i].houseFileName.find_last_of('.');
				string name = (houses[i].houseFileName).substr(0, index);
				name = name.substr(6, name.size() - 6);
				string trimmed = name.substr(0, 9);
				cout.width(10);
				cout << left << trimmed;
				cout << "|";
			}
		}
		cout << "AVG       |" << endl;
		// start printing scores for algorithms
		for (int i = 0; i < numOfAlgorithms; i++) {
			cout << string(dashes, '-') << endl;
			// if algorithm invalid: continue
			
			// else:
			// print algorithm file name.. scores... avg
			// cout << TODO: print algorithm file name
			cout << "|" << algorithmsNames.at(i) << " |";
			double avg = 0;

			for (int j = 0; j < numOfHouses; j++) {
				if (houses[j].isValidHouse == false)
					continue;

				cout.width(10);
				cout << right << scores[j][i];
				cout << "|";
				avg += scores[j][i];
			}

			avg /= numOfWorkingHouses;
			cout.width(10);
			cout << right << std::fixed << std::setprecision(2) << avg;
			cout << "|" << endl;
		}
		cout << string(dashes, '-') << endl;
	}

	getchar();
	// free houses
	for (int k = 0; k < numOfHouses; k++) {
		for (int i = 0; i < houses[k].rows; i++)
			delete[] houses[k].matrix[i];
		delete[] houses[k].matrix;
	}
	delete[] houses;
	delete[] algorithms;
}


int main(int argc, const char* argv[])
{
	int numOfHouses, numOfAlgorithms;
	int numOfWorkingHouses = 0, numOfWorkingAlgorithms;
	House* houses;
	map<string, int> config;
	// vector of length 3: [0] holds config path, [1] holds house path and [2] holds algorithm path
	// if not specified, place default
	vector<string> flags{defaultConfigPath, defaultHousePath, defaultAlgorithmPath};

	// only 1/3/5/7 arguments are acceptable (1/3/5 hold the flags, 2/4/6 hold the corresponding directories)
	if (argc == 2 || argc == 4 || argc == 6 || argc > 7) {
		cout << WRONG_ARGUMENTS_NUM << endl;
		return -1;
	}
	for (int i = 1; i < argc; i += 2) {
		if (!strcmp(argv[i], "-config"))
			flags[0] = argv[i+1];
		else if (!strcmp(argv[i], "-house_path"))
			flags[1] = argv[i+1];
		else if (!strcmp(argv[i], "-algorithm_path"))
			flags[2] = argv[i+1];
		else {
			cout << WRONG_ARGUMENTS << endl;
			return -1;
		}
	}

	// take care of config/house/algorithm files
	int handle = handleConfigFile(handleSlash((flags[0]).c_str()), config);
	if (handle < 0) {
		if (handle == -2)
			usageMessage(flags[0], flags[1], flags[2]);
		getchar();
		return -1;
	}

	// get number of houses in directory
	numOfHouses = getNumberOfHouses((flags[1]).c_str());
	if (numOfHouses == -1)
	{
		usageMessage(flags[0], flags[1], flags[2]);
		return -1;
	}
	houses = new House[numOfHouses];
	handle = handleHouseFiles(handleSlash((flags[1]).c_str()), numOfHouses, houses);
	if (handle == -1)
		return -1;

	// handle aglorithm files
	/*
	numOfAlgorithms = getNumberOfAlgorithms((flags[2]).c_str());
	if (numOfAlgorithms == -1)
	{
		// free houses
		for (int k = 0; k < numOfHouses; k++) {
			for (int i = 0; i < houses[k].rows; i++)
				delete[] houses[k].matrix[i];
			delete[] houses[k].matrix;
		}
		delete[] houses;
		usageMessage(flags[0], flags[1], flags[2]);
		return -1;
	}

	handle = handleAlgorithmFiles(flags[2], numOfAlgorithms);
	
	//startSimulation(houses, algorithms, numOfHouses, numOfAlgorithms, config);

	for (int i = 0; i < numOfHouses; i++)
		if (houses[i].isValidHouse)
			numOfWorkingHouses++;
	numOfWorkingAlgorithms = numOfAlgorithms;

	// if there were errors: print an empty single new line that would seperate the results table from the error list
	if (numOfHouses != numOfHouses || numOfAlgorithms != numOfWorkingAlgorithms) {
		cout << endl;
		cout << "Errors:" << endl;
		// print all errors
		for (int i = 0; i < numOfHouses; i++) {
			if (!(houses[i].isValidHouse)) {
				cout << houses[i].houseFileName << ":" << houses[i].error << endl;
			}
		}
		for (int i = 0; i < numOfAlgorithms; i++) {
			// print errors of algorithms
		}
	}
	*/
	startSimulation(houses, numOfHouses, 1, config);
	return 0;
}