// Simulation (main,cpp) : Defines the entry point for the console application.
//#include "stdafx.h"

#include "Simulator.h"

#define DEBUG 0
#define SHOW_SIMULATION_HOUSES 0

// assumes all algorithms that reach here are fine
void Simulator::startSimulation(House* houses, int numOfHouses, int numOfAlgorithms, map<string, int> config, AlgorithmRegistrar& registrar)
{
	// matrix (vector of vectors) of scores: scores[0] - scores of the first house on every algorithm and so on
	vector<vector<int>> scores(numOfHouses, vector<int>(numOfAlgorithms));
	vector<string> walkingIntoWallsErrors;
	auto algorithms = registrar.getAlgorithms();
	auto& algorithmNames = registrar.getAlgorithmNames();
	bool is_back_in_docking;

	// specific to house
	int max_steps;
	int simulation_num_steps;
	int batteryCapacity;
	int batteryConsumptionRate;
	int batteryRechargeRate;

	// the house list may contain defected houses -> so we skip them, and count how many of them are good
	int numOfWorkingHouses = 0;

	House* curHouses;

	// iterate over all houses
	for (int k = 0; k < numOfHouses; k++)
	{
		if (!(houses[k].isValidHouse))
			continue;
		numOfWorkingHouses++;

		// just checking to see if it worked (print the house) - for debug purpose
		string space = " ";
		if (houses[k].rows > 60 || houses[k].cols > 40)
			space = "";
		if (DEBUG) {
			if (houses[k].matrix != NULL) {
				for (int i = 0; i < houses[k].rows; i++) {
					for (int j = 0; j < houses[k].cols; j++) {
						cout << houses[k].matrix[i][j] << space;
					}
					cout << endl;
				}
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
		for (int l = 0; l < numOfAlgorithms; l++) {
			copyHouse(curHouses[l], houses[k]);
			sensors.emplace_back(Sensor(&curHouses[l]));
		}

		int i = 0;
		for (auto& algorithm : algorithms)
		{
			algorithm->setSensor(sensors[i]);
			algorithm->setConfiguration(config);
			i++;
		}
		
		bool already_alerted_more_steps = false;
		max_steps = houses[k].maxSteps;
		batteryCapacity = (config.find("BatteryCapacity"))->second;
		for (int i = 0; i < numOfAlgorithms; i++)
			curBattery[i] = batteryCapacity;
		batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;
		batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;
		simulation_num_steps = 0;

		if (DEBUG) {
			cout << endl;
			printHouseWithRobot(houses[k]);
		}

		while (true) {
			simulation_num_steps++;
			if (SHOW_SIMULATION_HOUSES) {
				getchar();
				cout << "Step " << simulation_num_steps << endl;
				//Sleep(3000);
			}
			// simulate one step for each algorithm
			int l = -1;
			list<string>::const_iterator nameIterator = algorithmNames.begin();
			for (auto& algorithm : algorithms) {
				// increase for the next algorithm
				l++;
				if (if_end[l] == true)
				{
					nameIterator++;
					continue;
				}

				Direction direction = algorithm->step();

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
					// make the error note to be printed later (at the end after all other errors)
					int index = static_cast<int>((*nameIterator).find(".so"));
					string name = (*nameIterator).substr(0, index);
					string wallError = "Algorithm ";
					wallError += name;
					wallError += " when running on House ";
					index = static_cast<int>(curHouses[l].houseFileName.find_last_of('.'));
					name = (curHouses[l].houseFileName).substr(0, index);
					name = name.substr(6, name.size() - 6);
					wallError += name;
					wallError += " went on a wall in step ";
					wallError += to_string(simulation_num_steps);
					walkingIntoWallsErrors.emplace_back(wallError);
					if (DEBUG)
						cout << INTO_WALL << endl;
					nameIterator++;
					continue;
				}

				// for debug purpose
				if (SHOW_SIMULATION_HOUSES) {
					cout << "Robot(" << (*nameIterator) << ") Battery: " << curBattery[l] << endl;
					printHouseWithRobot(curHouses[l]);
				}

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
					nameIterator++;
					continue;
				}
				if (curBattery[l] <= 0) {
					if (DEBUG)
						cout << BATTERY_DEAD << endl; // for debug purpose
					if_end[l] = true;
					finished++;
					nameIterator++;
					continue;
				}
				nameIterator++;
			}
			if (cur_stage_winners > 0)
				cur_position = MIN(4, cur_position + cur_stage_winners);
			cur_stage_winners = 0;

			// - MAX-STEPS-AFTER-WINNNER ALERT -
			if (!already_alerted_more_steps) {
				// let all the other algorithms (that did not win in the this last move) know 'MaxStepsAfterWinner'
				// alret them only at the first round when some algorithm wins
				// the condition is true ONLY on the first round when some algorithm wins
				if (winner_num_steps == simulation_num_steps) {
					// if someone wins, max_steps is already updated in the loop, so it's simply a subtraction
					int alert_more_steps = max_steps - simulation_num_steps;
					int l = 0;
					for (auto& algorithm : algorithms) {
						// alert only algorithms that did not win / die (they're battery finished before charging)
						if (if_end[l] == false) {
							algorithm->aboutToFinish(alert_more_steps);
						}
						l++;
					}
					already_alerted_more_steps = true;
					if (DEBUG)
						cout << endl << "ALERT TO ALL ALGORITHMS: more steps = " << alert_more_steps << endl;
				}
				// other case -> no one won but there are 'maxstepsafterwinner' more steps till the end
				// alert all algorithms
				else if (!is_winner && ((max_steps - simulation_num_steps) == config["MaxStepsAfterWinner"])) {
					int l = 0;
					for (auto& algorithm : algorithms) {
						algorithm->aboutToFinish(config["MaxStepsAfterWinner"]);
						l++;
					}
					already_alerted_more_steps = true;
					if (DEBUG)
						cout << endl << "ALERT TO ALL ALGORITHMS: more steps = " << config["MaxStepsAfterWinner"] << endl;
				}
			}

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
		if (DEBUG)
			getchar();

		// delete cur houses and sensors
		for (int l = 0; l < numOfAlgorithms; l++) {
			for (int i = 0; i < curHouses[l].rows; i++)
				delete[] curHouses[l].matrix[i];
			delete[] curHouses[l].matrix;
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
				int index = static_cast<int>(houses[i].houseFileName.find(".house"));
				string name = (houses[i].houseFileName).substr(0, index);
				index = name.find_last_of('/');
				if (index != -1)
					name = name.substr(index + 1, name.size() - index - 1);
				string trimmed = name.substr(0, 9);
				cout.width(10);
				cout << left << trimmed;
				cout << "|";
			}
		}
		cout << "AVG       |" << endl;
		// start printing scores for algorithms
		list<string>::const_iterator nameIterator = algorithmNames.begin();
		for (int i = 0; i < numOfAlgorithms; i++) {
			cout << string(dashes, '-') << endl;
			// print algorithm file name.. scores... avg
			int index = static_cast<int>((*nameIterator).find(".so"));
			string name = (*nameIterator).substr(0, index);
			string trimmed = name.substr(0, 12);
			cout << "|";
			cout.width(13);
			cout << left << trimmed;
			cout << "|";
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
			nameIterator++;
		}
		cout << string(dashes, '-') << endl;
	}

	// print Errors
	// if there were errors: print an empty single new line that would seperate the results table from the error list
	if (numOfHouses != numOfWorkingHouses || registrar.getErrorsList().size() > 0 || walkingIntoWallsErrors.size() > 0) {
		cout << endl;
		cout << "Errors:" << endl;
		// print all house errors
		for (int i = 0; i < numOfHouses; i++) {
			if (!(houses[i].isValidHouse)) {
				cout << houses[i].houseFileName << ": " << houses[i].error << endl;
			}
		}
		// print all algorithms errors
		list<string> errs = registrar.getErrorsList();
		for (list<string>::iterator it = errs.begin(); it != errs.end(); ++it) {
			cout << *it << endl;
		}
		// print walking into walls errors
		for (vector<string>::iterator it = walkingIntoWallsErrors.begin(); it != walkingIntoWallsErrors.end(); ++it) {
			cout << *it << endl;
		}
	}
}


int main(int argc, const char* argv[])
{
	Simulator simulator;
	int numOfHouses;
	int numOfAlgorithms;
	House* houses = nullptr;
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	std::map<string, int> config = {};
	// vector of length 3: [0] holds config path, [1] holds house path and [2] holds algorithm path
	// if not specified, place default
	vector<string> flags{ defaultConfigPath, defaultHousePath, defaultAlgorithmPath };

	// ignore non-interesting flags
	for (int i = 1; i < argc - 1; ) {
		if (!strcmp(argv[i], "-config")) {
			flags[0] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-house_path")) {
			flags[1] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-algorithm_path")) {
			flags[2] = argv[i + 1];
			i += 2;
		}
		else {
			i++;
		}
	}

	// handle config file
	int handle = handleConfigFile(handleSlash((flags[0]).c_str()), config);
	if (handle < 0) {
		if (handle == -2)
			cout << USAGE << endl;
		return -1;
	}

	// handle algorithm files
	// [exercise says: - in case the directory is defect -> return
	//				   - in case the directory is empty -> return
	//				   - in case the directory is missing -> search recursively in the working directory for algorithms]
	numOfAlgorithms = getNumberOfPotentialAlgorithms(flags[2]);
	if (numOfAlgorithms == -1 || numOfAlgorithms == 0) {
		cout << USAGE << endl;
		return -1;
	}
	if (numOfAlgorithms == -2) { // search recursive in the working directory
		if (flags[2].empty()) { // already searched in there.. -> return
			cout << USAGE << endl;
			return -1;
		}
		numOfAlgorithms = getNumberOfPotentialAlgorithms(""); // search in the working directory
		if (numOfAlgorithms <= 0) {
			cout << USAGE << endl;
			return -1;
		}
		else {
			// update flags[2] to the working directory -> it contains algorithms that should be loaded
			flags[2] = "";
		}
	}
	handle = handleAlgorithmFiles(handleSlash(flags[2].c_str()), numOfAlgorithms, registrar);
	if (handle < 0) {
		if (handle == -1) {
			cout << USAGE << endl;
		}
		return -1;
	}

	// handle house files
	// [exercise says: - in case the directory is defect -> return
	//				   - in case the directory is empty -> return
	//				   - in case the directory is missing -> search recursively in the working directory for algorithms]
	numOfHouses = getNumberOfHouses(handleSlash((flags[1]).c_str()));
	if (numOfHouses == -1 || numOfHouses == 0) {
		cout << USAGE << endl;
		return -1;
	}
	if (numOfHouses == -2) { // search recursive in the working directory
		if (flags[1].empty()) { // already searched in there.. -> return
			cout << USAGE << endl;
			return -1;
		}
		numOfHouses = getNumberOfHouses(""); // search in the working directory
		if (numOfHouses <= 0) {
			cout << USAGE << endl;
			return -1;
		}
		else {
			// update flags[1] to the working directory -> it contains the houses that should be read
			flags[1] = "";
		}
	}
	houses = new House[numOfHouses];
	handle = handleHouseFiles(handleSlash((flags[1]).c_str()), numOfHouses, houses);
	if (handle < 0) {
		if (handle == -1) {
			cout << USAGE << endl;
		}
		freeHouses(houses, numOfHouses);
		return -1;
	}
	numOfAlgorithms = registrar.getAlgorithmNames().size();
	
	simulator.startSimulation(houses, numOfHouses, numOfAlgorithms, config, registrar);
	
	// free houses
	freeHouses(houses, numOfHouses);
	// free dynamic loaded files
	registrar.clearFactories();
	registrar.clearHndls();

	return 0;
}