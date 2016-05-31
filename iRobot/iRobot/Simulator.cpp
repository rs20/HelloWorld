// Simulation (main.cpp) : Defines the entry point for the console application.
#include "Simulator.h"

// use the debug macros only when working with one thread ONLY!
#define DEBUG 0
#define SHOW_SIMULATION_HOUSES 0

int main(int argc, const char* argv[])
{
	Simulator simulator;
	simulator.handleArguments(argc, argv);
	if (simulator.handleAll())
		return -1;
	simulator.startSimulation();
	simulator.end();
	return 0;
}


void Simulator::handleArguments(int argc, const char* argv[])
{
	// ignore non-interesting flags
	for (int i = 1; i < argc; ) {
		if (!strcmp(argv[i], "-config") && i < argc - 1) {
			flags[0] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-score_formula") && i < argc - 1) {
			flags[1] = argv[i + 1];
			i += 2;
			score_loaded = true;
		}
		else if (!strcmp(argv[i], "-house_path") && i < argc - 1) {
			flags[2] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-algorithm_path") && i < argc - 1) {
			flags[3] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-threads") && i < argc - 1) {
			flags[4] = argv[i + 1];
			i += 2;
		}
		else if (!strcmp(argv[i], "-video")) {
			isFlagVideoUp = true;
			i++;
		}
		else {
			i++;
		}
	}
}

int Simulator::handleAll()
{
	if (handleConfiguration())
		return -1;
	if (handleScore())
		return -1;
	if (handleAlgorithms())
		return -1;
	if (handleHouses())
		return -1;
	handleThreads();
	if (handleVideo())
		return -1;
	return 0;
}

int Simulator::handleConfiguration()
{
	// handle config file
	int handle = handleConfigFile(handleSlash((flags[0]).c_str()), config);
	if (handle < 0) {
		if (handle == -2)
		{
			cout << USAGE << endl;
			cout << ERROR_FIND_CONFIG_FILE << fullPath(handleSlash((flags[0]).c_str())) << endl;
		}
		return -1;
	}
	return 0;
}

int Simulator::handleScore()
{
	if (!score_loaded)
	{
		score_function = NULL;
		return 0;
	}

	int handle = handleScoreFile(handleSlash((flags[1]).c_str()), score_hndl, &score_function);
	if (handle < 0) {
		if (handle == -2) {
			cout << USAGE << endl;
			std::cout << ERROR_FIND_SCORE_FILE << fullPath(handleSlash((flags[1]).c_str())) << std::endl;
		}
		return -1;
	}
	return 0;
}

int Simulator::handleAlgorithms()
{
	// handle algorithm files
	// [exercise says: - in case the directory is defect -> return
	//				   - in case the directory is empty -> return
	//				   - in case the directory is missing -> search recursively in the working directory for algorithms]
	int handle = 0;
	numOfAlgorithms = getNumberOfPotentialAlgorithms(flags[3]);
	if (numOfAlgorithms == -1 || numOfAlgorithms == 0) {
		cout << USAGE << endl;
		cout << ERROR_FIND_ALGORITHM_FILES << fullPath(flags[3]) << endl; 
		return -1;
	}
	else if (numOfAlgorithms == -2) {
		// no usage print is necessary
		return -1;
	}
	handle = handleAlgorithmFiles(handleSlash(flags[3].c_str()), numOfAlgorithms, registrar);
	if (handle < 0) {
		if (handle == -1) {
			cout << USAGE << endl;
		}
		return -1;
	}
	walkingIntoWallsErrors = make_unique<string[]>(numOfAlgorithms);
	numOfAlgorithms = registrar.getAlgorithmNames().size();
	return 0;
}

// handlehouses should now only list the house file names in the vector and not read them!
int Simulator::handleHouses()
{
	// handle house files
	// [exercise says: - in case the directory is defect -> return
	//				   - in case the directory is empty -> return
	//				   - in case the directory is missing -> search recursively in the working directory for algorithms]
	numOfHouses = getNumberOfHouses(handleSlash((flags[2]).c_str()), houseFileNames);
	if (numOfHouses == -1 || numOfHouses == 0) {
		cout << USAGE << endl;
		cout << ERROR_FIND_HOUSE_FILES << fullPath(flags[2]) << endl;
		return -1;
	}
	isValidHouses = make_unique<bool[]>(numOfHouses);
	houseErrors = make_unique<string[]>(numOfHouses);
	return 0;
}

void Simulator::handleThreads()
{
	// default number of threads in flags[4] is "1"
	char* end;
	strtol(flags[4].c_str(), &end, 10); // if the string doesn't represent a number, define the number to be 1
	if (*end != 0) {
		numOfThreads = 1;
		return;
	}
	numOfThreads = atoi(flags[4].c_str());
	if (numOfThreads <= 0) // number of threads shall be a positive number
		numOfThreads = 1;
	else if (numOfThreads > numOfHouses) // number of threads shall not exceed number of houses
		numOfThreads = numOfHouses;
}

int Simulator::handleVideo()
{
	if (isFlagVideoUp && (numOfThreads > 1))
	{
		cout << ERROR_VIDEO << endl;
		return -1;
	}
	return 0;
}

void Simulator::startSimulation()
{
	// for each algorithm -> initialize array of scores for each house
	for (string algorithmName : registrar.getAlgorithmNames())
		scores[algorithmName] = make_unique<int[]>(numOfHouses);
	
	if (numOfThreads > 1)
	{
		// create threads and start them
		vector<unique_ptr<thread>> threads(numOfThreads);
		for (auto& thread_ptr : threads)
			thread_ptr = make_unique<thread>(&Simulator::threadSimulation, this);

		// join all threads
		for (auto& thread_ptr : threads)
			thread_ptr->join();
	}
	else // numOfThreads = 1
	{
		threadSimulation();
	}
	

	// print results
	printScores();
	printErrors();
}

void Simulator::threadSimulation()
{
	// assign houses to the threads (nextHouse is atomic integer)
	for (int houseIndex = nextHouse.fetch_add(1, std::memory_order_relaxed); houseIndex < numOfHouses; houseIndex = nextHouse.fetch_add(1, std::memory_order_relaxed)) { 
		runThreadOnHouse(houseIndex);
	}
}

void Simulator::runThreadOnHouse(int houseIndex)
{
	string houseFileName = houseFileNames[houseIndex];
	// read house file and run all algorithms on the specific house
	unique_ptr<House> house = make_unique<House>();
	int handle = readHouseFile(houseIndex, houseFileName, houseErrors.get(), house.get()); // pass ptr without releasing (house will be updated)
	if (handle) {
		// error
		isValidHouses[houseIndex] = false; // (default is false anyway)
		return;
	}
	numOfWorkingHouses++; // atomic increase
	isValidHouses[houseIndex] = true;
	// automatic win for each algorithm if the dirt in the house == 0
	if (house->sumOfDirt == 0) {
		map<string, int> autoWinScore;
		autoWinScore["simulation_steps"] = 0;
		autoWinScore["winner_num_steps"] = 0;
		autoWinScore["this_num_steps"] = 0;
		autoWinScore["sum_dirt_in_house"] = 0;
		autoWinScore["dirt_collected"] = 0;
		autoWinScore["is_back_in_docking"] = 1;
		autoWinScore["actual_position_in_competition"] = 1;
		auto nameIterator = registrar.getAlgorithmNames().begin();
		for (int i = 0; i < numOfAlgorithms; i++) {
			if (score_function != NULL) {
				scores[*nameIterator][houseIndex] = (*score_function)(autoWinScore);
			}
			else {
				scores[*nameIterator][houseIndex] = score(autoWinScore);
			}
		}
		return; // that's all
	}

	// new instance of all algorithms
	AlgorithmRegistrar& curRegistrar = AlgorithmRegistrar::getInstance();
	auto algorithms = curRegistrar.getAlgorithms();
	auto& algorithmNames = curRegistrar.getAlgorithmNames();
	auto nameIterator = algorithmNames.begin();
	vector<bool> if_end(numOfAlgorithms);
	vector<bool> into_wall(numOfAlgorithms);
	vector<int> curBattery(numOfAlgorithms);
	vector<int> numSteps(numOfAlgorithms);
	vector<int> positionInComp(numOfAlgorithms, 10); // default position is 10
	vector<Direction> lastMoves(numOfAlgorithms, Direction::Stay);
	int simulation_num_steps = 0;
	int max_steps = house->maxSteps;
	int batteryCapacity = (config.find("BatteryCapacity"))->second;
	int batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;
	int batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;
	for (int i = 0; i < numOfAlgorithms; i++)
		curBattery[i] = batteryCapacity;
	bool is_winner = false;
	int winner_num_steps = -1;
	int cur_stage_winners = 0;
	int cur_position = 1;
	int finished = 0;
	bool already_alerted_more_steps = false;
	int algIndex;
	// make a copy of the current house for every algorithm and assign a sensor to it
	unique_ptr<House[]> curHouses = make_unique<House[]>(numOfAlgorithms);
	vector<Sensor> sensors;
	for (int l = 0; l < numOfAlgorithms; l++) {
		copyHouse(curHouses[l], house.get());
		sensors.emplace_back(Sensor(&curHouses[l]));
	}
	int i = 0;
	for (auto& algorithm : algorithms)
	{
		algorithm->setSensor(sensors[i]);
		algorithm->setConfiguration(config);
		i++;
	}

	if (DEBUG) {
		// print the house with D (no R) - for debugging
		string space = " ";
		if (house->rows > 60 || house->cols > 40)
			space = "";
		if (house->matrix != NULL) {
			for (int i = 0; i < house->rows; i++) {
				for (int j = 0; j < house->cols; j++) {
					cout << house->matrix[i][j] << space;
				}
				cout << endl;
			}
		}
		// print with robot R (no D)
		cout << endl;
		printHouseWithRobot(curHouses[0]); // house == curHouses[0] == ... == curHouses[numOfAlgorithms-1]
	}

	while (true) {
		simulation_num_steps++;
		if (SHOW_SIMULATION_HOUSES) {
			getchar();
			cout << "Step " << simulation_num_steps << endl;
		}
		// simulate one step for each algorithm
		algIndex = -1;
		nameIterator = algorithmNames.begin();
		for (auto& algorithm : algorithms) {
			// increase for the next algorithm
			algIndex++;
			if (if_end[algIndex] == true) {
				nameIterator++;
				continue;
			}
			// pass last move of the algorithm and update the new one
			Direction direction = algorithm->step(lastMoves[algIndex]);
			lastMoves[algIndex] = direction;

			// if leaving docking station -> load battery
			if (curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] == 'D') {
				curBattery[algIndex] = MIN(batteryCapacity, curBattery[algIndex] + batteryRechargeRate);
			}
			// consume battery only if did not start the move from the docking station
			// staying or starting the move from the docking station does not consume battery
			if (curHouses[algIndex].robot != curHouses[algIndex].docking)
				curBattery[algIndex] -= batteryConsumptionRate;

			// make the step on the current house of the algorithm
			switch (direction)
			{
			case static_cast<Direction>(0) :
				curHouses[algIndex].robot.col++;
				break;
			case static_cast<Direction>(1) :
				curHouses[algIndex].robot.col--;
				break;
			case static_cast<Direction>(2) :
				curHouses[algIndex].robot.row++;
				break;
			case static_cast<Direction>(3) :
				curHouses[algIndex].robot.row--;
				break;
			default:
				break;
				// do nothing for 'Stay'
			}

			//cleaning dust when entering a cell
			if (curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] > '0' && curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] <= '9') {
				curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] = curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] - 1;
				curHouses[algIndex].sumOfDirt--;
			}

			// create a snapshot of the current house if desired && there was no previous error with creating a folder for the current alg+home
			// note that we do take a snapshot of the case in which a robot gets into a wall (that's ok as amir said)
			if (isFlagVideoUp && !curHouses[algIndex].folderError) {
				curHouses[algIndex].montage(*nameIterator, videoErrors);
			}

			// walked into a wall -> stop the algorithm from running. its score will be zero
			if (curHouses[algIndex].matrix[curHouses[algIndex].robot.row][curHouses[algIndex].robot.col] == 'W') {
				into_wall[algIndex] = true;
				if_end[algIndex] = true;
				finished++;
				// make the error note to be printed later (at the end after all other errors)
				int index = static_cast<int>((*nameIterator).find(".so"));
				string name = (*nameIterator).substr(0, index);
				string wallError = "Algorithm ";
				wallError += name;
				wallError += " when running on House ";
				index = static_cast<int>(curHouses[algIndex].houseFileName.find_last_of('.'));
				name = (curHouses[algIndex].houseFileName).substr(0, index);
				name = name.substr(6, name.size() - 6);
				wallError += name;
				wallError += " went on a wall in step ";
				wallError += to_string(simulation_num_steps);
				walkingIntoWallsErrors[houseIndex] = wallError;
				algorithmIntoWall = true;
				if (DEBUG)
					cout << INTO_WALL << endl;
				nameIterator++;
				continue;
			}

			// for debug purpose
			if (SHOW_SIMULATION_HOUSES) {
				cout << "Robot(" << (*nameIterator) << ") Battery: " << curBattery[algIndex] << endl;
				printHouseWithRobot(curHouses[algIndex]);
			}

			if (curHouses[algIndex].sumOfDirt == 0 && curHouses[algIndex].robot == curHouses[algIndex].docking) {
				if (DEBUG)
					cout << "Robot wins (cleaned the whole house in the limited time)." << endl; //  for debug purpose
				if_end[algIndex] = true;
				cur_stage_winners++;
				if (!is_winner) {
					is_winner = true;
					winner_num_steps = simulation_num_steps;
					max_steps = MIN(max_steps, simulation_num_steps + config["MaxStepsAfterWinner"]);
				}
				finished++;
				positionInComp[algIndex] = cur_position;
				numSteps[algIndex] = simulation_num_steps;
				nameIterator++;
				continue;
			}
			if (curBattery[algIndex] <= 0) {
				if (DEBUG)
					cout << BATTERY_DEAD << endl; // for debug purpose
				if_end[algIndex] = true;
				finished++;
				nameIterator++;
				continue;
			}
			nameIterator++;
		}
		// finished one step of each algorithm
		if (cur_stage_winners > 0)
			cur_position = cur_position + cur_stage_winners;
		cur_stage_winners = 0;

		// - MAX-STEPS-AFTER-WINNNER ALERT -
		if (!already_alerted_more_steps) {
			// let all the other algorithms (that did not win in the this last move) know 'MaxStepsAfterWinner'
			// alret them only at the first round when some algorithm wins
			// the condition is true ONLY on the first round when some algorithm wins
			if (winner_num_steps == simulation_num_steps) {
				// if someone wins, max_steps is already updated in the loop, so it's simply a subtraction
				int alert_more_steps = max_steps - simulation_num_steps;
				algIndex = 0;
				for (auto& algorithm : algorithms) {
					// alert only algorithms that did not win / die (they're battery finished before charging)
					if (if_end[algIndex] == false) {
						algorithm->aboutToFinish(alert_more_steps);
					}
					algIndex++;
				}
				already_alerted_more_steps = true;
				if (DEBUG)
					cout << endl << "ALERT TO ALL ALGORITHMS: more steps = " << alert_more_steps << endl;
			}
			// other case -> no one won but there are 'maxstepsafterwinner' more steps till the end
			// alert all algorithms
			else if (!is_winner && ((max_steps - simulation_num_steps) == config["MaxStepsAfterWinner"])) {
				algIndex = 0;
				for (auto& algorithm : algorithms) {
					// alert only algorithms that did not die (they're battery finished before charging / went into a wall)
					if (if_end[algIndex] == false) {
						algorithm->aboutToFinish(config["MaxStepsAfterWinner"]);
					}
					algIndex++;
				}
				already_alerted_more_steps = true;
				if (DEBUG)
					cout << endl << "ALERT TO ALL ALGORITHMS: more steps = " << config["MaxStepsAfterWinner"] << endl;
			}
		}
		// end of game
		if (finished == numOfAlgorithms || simulation_num_steps == max_steps) {
			if (DEBUG)
				cout << NO_MORE_MOVES << endl;  // for debug purpose
			for (algIndex = 0; algIndex < numOfAlgorithms; algIndex++) {
				// if didn't change -> he didn't win. set number of steps to simulation steps
				if (numSteps[algIndex] == 0) {
					numSteps[algIndex] = simulation_num_steps;
				}
			}
			break;
		}

	}

	// score the algorithms on the house
	// if none won -> winner num steps = simulation num steps
	// call the right score method (Score.h or calc_score from the loaded score_formula.so)
	// (call calc_score if score_loaded==true (field in Simulator.h) and call the score method in Score.h when it's false)
	if (winner_num_steps == -1)
		winner_num_steps = simulation_num_steps;
	nameIterator = algorithmNames.begin();
	for (int algIndex = 0; algIndex < numOfAlgorithms; algIndex++) {
		int is_back_in_docking = (curHouses[algIndex].robot == curHouses[algIndex].docking) ? true : false;
		if (into_wall[algIndex] == true) // if walked into a wall, score=0
			scores[*nameIterator][houseIndex] = 0;
		else {
			map<string, int> score_params;
			score_params["simulation_steps"] = simulation_num_steps;
			score_params["winner_num_steps"] = winner_num_steps;
			score_params["this_num_steps"] = numSteps[algIndex];
			score_params["sum_dirt_in_house"] = curHouses[algIndex].initialSumOfDirt;
			score_params["dirt_collected"] = curHouses[algIndex].initialSumOfDirt - curHouses[algIndex].sumOfDirt;
			score_params["is_back_in_docking"] = is_back_in_docking;
			if (curHouses[algIndex].sumOfDirt == 0 && is_back_in_docking) {
				score_params["actual_position_in_competition"] = positionInComp[algIndex];
			}
			else {
				score_params["actual_position_in_competition"] = 10;
			}
			if (score_function != NULL) {
				scores[*nameIterator][houseIndex] = (*score_function)(score_params);
			}
			else {
				scores[*nameIterator][houseIndex] = score(score_params);
			}

			if (scores[*nameIterator][houseIndex] == -1)
				isErrorInScoreCalc = true;
		}
		nameIterator++;
	}

	// create the videos for all the algorithms on the house
	if (isFlagVideoUp)
	{
		algIndex = 0;
		nameIterator = algorithmNames.begin();
		string simulationDir;
		string imagesExpression;
		while (nameIterator != algorithmNames.end()) {
			// make a video only when the folder creation was successfull
			if (!curHouses[algIndex].folderError) {
				// image creation error
				if (curHouses[algIndex].imageErrors > 0) {
					string error_msg = "Error: In the simulation " + *nameIterator + ", " + curHouses[algIndex].houseFileName + ": the creation of " + curHouses[algIndex].imageErrors + " images was failed";
					videoErrors.push_back(error_msg);
				}
				// make a video only when at least one snapshot was created successfully
				if (curHouses[algIndex].createVideo) {
					simulationDir = "simulations/" + *nameIterator + "_" + curHouses[algIndex].houseFileName + "/";
					imagesExpression = simulationDir + "image%5d.jpg";
					if (Encoder::encode(imagesExpression, *nameIterator + "_" + curHouses[algIndex].houseFileName + ".mpg")) { // video creation fail
						string error_msg = "Error: In the simulation " + *nameIterator + ", " + curHouses[algIndex].houseFileName + ": video file creation failed";
						videoErrors.push_back(error_msg);
					}
				}
			}
			nameIterator++;
			algIndex++;
		}
	}

	if (DEBUG)
		getchar();
}

void Simulator::printScores()
{
	vector<pair<int, pair<string, vector<int>>>> avg_scores; // avg score of an algorithm + its name + its scores
	if (numOfWorkingHouses > 0) {
		// print first row
		int dashes = 15 + 11 * (numOfWorkingHouses + 1);
		cout << string(dashes, '-') << endl;
		cout << "|" << string(13, ' ') << "|";
		for (int houseIndex = 0; houseIndex < numOfHouses; houseIndex++) {
			if (isValidHouses[houseIndex]) {
				int index = static_cast<int>(houseFileNames[houseIndex].find(".house"));
				string name = (houseFileNames[houseIndex]).substr(0, index);
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
		auto nameIterator = registrar.getAlgorithmNames().begin();
		for (int algIndex = 0; algIndex < numOfAlgorithms; algIndex++) {
			vector<int> this_alg_scores;
			double avg = 0;
			for (int houseIndex = 0; houseIndex < numOfHouses; houseIndex++) {
				if (!(isValidHouses[houseIndex]))
					continue;
				avg += scores[*nameIterator][houseIndex];
				this_alg_scores.push_back(scores[*nameIterator][houseIndex]);
			}
			avg /= numOfWorkingHouses;
			avg_scores.push_back({ avg,{ *nameIterator, this_alg_scores } });
			nameIterator++;
		}
		// sort by score (ascending order)
		sort(avg_scores.begin(), avg_scores.end());
		// iterate the vector in backwards order
		for (vector<pair<int, pair<string, vector<int>>>>::iterator it = avg_scores.end() - 1; it != avg_scores.begin() - 1; it--) {
			cout << string(dashes, '-') << endl;
			// print algorithm file name.. scores... avg
			string name = ((*it).second).first;
			double avg = (*it).first;
			int index = static_cast<int>(name.find(".so"));
			name = name.substr(0, index);
			string trimmed = name.substr(0, 12);
			cout << "|";
			cout.width(13);
			cout << left << trimmed;
			cout << "|";
			vector<int> this_alg_scores = ((*it).second).second;
			for (vector<int>::iterator score_it = this_alg_scores.begin(); score_it != this_alg_scores.end(); score_it++) {
				cout.width(10);
				cout << right << *score_it;
				cout << "|";
			}
			cout.width(10);
			cout << right << std::fixed << std::setprecision(2) << avg;
			cout << "|" << endl;
		}
		cout << string(dashes, '-') << endl;
	}
}

void Simulator::printErrors()
{
	// all houses are defected
	if (numOfWorkingHouses == 0) {
		string fpath = fullPath(flags[2]);
		std::cout << "All house files in target folder " << "'" << fpath << "'" << " cannot be opened or are invalid:" << std::endl;
		for (int houseIndex = 0; houseIndex < numOfHouses; houseIndex++) {
			std::cout << houseFileNames[houseIndex] << ": " << houseErrors[houseIndex] << std::endl;
		}
		return;
	}

	// if there were errors: print an empty single new line that would seperate the results table from the error list
	if (numOfHouses != numOfWorkingHouses || registrar.getErrorsList().size() > 0 || algorithmIntoWall) {
		cout << endl;
		cout << "Errors:" << endl;
		// print all house errors
		for (int houseIndex = 0; houseIndex < numOfHouses; houseIndex++) {
			if (!(isValidHouses[houseIndex])) {
				cout << houseFileNames[houseIndex] << ": " << houseErrors[houseIndex] << endl;
			}
		}
		// print all algorithms errors
		auto errs = registrar.getErrorsList();
		for (auto it = errs.begin(); it != errs.end(); ++it) {
			cout << *it << endl;
		}
		// print walking into walls errors
		for (int algIndex = 0; algIndex < numOfAlgorithms; algIndex++) {
			if (!(walkingIntoWallsErrors[algIndex].empty()))
				cout << walkingIntoWallsErrors[algIndex] << endl;
		}
		// print video errors
		for (auto it = videoErrors.begin(); it != videoErrors.end(); it++) {
			cout << *it << endl;
		}
	}

	// print score error:
	if (isErrorInScoreCalc) {
		cout << ERROR_CALCULATE_SCORE << endl;
	}
}

void Simulator::end()
{
	// free houses
	//houses.reset();
	// free dynamic loaded files
	registrar.clearFactories();
	registrar.clearHndls();
	if (score_hndl != NULL)
		dlclose(score_hndl);
}