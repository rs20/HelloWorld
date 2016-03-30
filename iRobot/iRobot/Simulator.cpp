// Simulation (main,cpp) : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Auxiliary.h"
#include "Simulator.h"
#include "Score.h"

void startSimulation(House* houses, int numOfHouses, map<string, int> config)
{
	int* scores;
	int is_back_in_docking;
	int max_steps;
	int this_num_steps;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;

	// iterate over all houses
	scores = new int[numOfHouses];
	for (int k = 0; k < numOfHouses; k++)
	{
		if (!(houses[k].isValidHouse))
			continue;
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
		Sensor sensor(&houses[k]);
		Algorithm alg;
		alg.setSensor(sensor);
		alg.setConfiguration(config);
		max_steps = (config.find("MaxSteps"))->second;
		batteryCapacity = (config.find("BatteryCapacity"))->second;;
		curBattery = batteryCapacity;
		batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;
		batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;

		//Sleep(1000);
		//cout << "iRobot starts cleaning the house..(Battery: " << curBattery << ")" << endl;
		//printHouseWithRobot(houses[k]);

		// simulate the algorithm on the house:
		bool goodStep = true;
		this_num_steps = 0;

		// if no dirt in house -> automatic win
		if (houses[k].sumOfDirt != 0)
		{
			while (true) {
				this_num_steps++;
				//Sleep(1000);

				Direction direction = alg.step();

				//cleaning dust if there is any.
				if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] > '0' && houses[k].matrix[houses[k].robotRow][houses[k].robotCol] <= '9') {
					houses[k].matrix[houses[k].robotRow][houses[k].robotCol] = houses[k].matrix[houses[k].robotRow][houses[k].robotCol] - 1;
					houses[k].sumOfDirt--;
				}

				if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] == 'D') {
					curBattery = MIN(batteryCapacity, curBattery + batteryRechargeRate);
					//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CHARGING BATTERY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl; // for debug purpose
				}


				// consume battery only if did not start the move from the docking station
				// as amir said: staying or starting the move from the docking station does not consume battery
				if (houses[k].robotRow != houses[k].dockingRow || houses[k].robotCol != houses[k].dockingCol)
					curBattery -= batteryConsumptionRate;

				switch (direction)
				{
				case static_cast<Direction>(0) :
					houses[k].robotCol++;
					break;
				case static_cast<Direction>(1) :
					houses[k].robotCol--;
					break;
				case static_cast<Direction>(2) :
					houses[k].robotRow++;
					break;
				case static_cast<Direction>(3) :
					houses[k].robotRow--;
					break;
				default:
					break;
				// do nothing for 'Stay'
				}

				if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] == 'W') { // walked into a wall -> stop the algorithm immediately. its score will be zero
					goodStep = false;
					cout << INTO_WALL << endl;
					break;
				}

				// for debug purpose
				cout << "Step " << this_num_steps << endl;
				cout << "Robot Battery: " << curBattery << endl;
				printHouseWithRobot(houses[k]);

				if (houses[k].sumOfDirt == 0 && houses[k].robotRow == houses[k].dockingRow && houses[k].robotCol == houses[k].dockingCol) {
					//cout << "Robot wins (cleaned the whole house in the limited time)." << endl; //  for debug purpose
					break;
				}
				if (curBattery <= 0) {
					//cout << BATTERY_DEAD << endl; // for debug purpose
					break;
				}
				if (this_num_steps == max_steps) {
					//cout << NO_MORE_MOVES << endl;  // for debug purpose
					break;
				}
			}
		}

		// score the algorithm on the house
		is_back_in_docking = (houses[k].robotRow == houses[k].dockingRow && houses[k].robotCol == houses[k].dockingCol) ? 1 : 0;
		if (goodStep == false) // if walked into a wall, score=0
			scores[k] = 0;
		else if (houses[k].sumOfDirt == 0 && is_back_in_docking)
			scores[k] = score(1, this_num_steps, this_num_steps, houses[k].initialSumOfDirt - houses[k].sumOfDirt, houses[k].initialSumOfDirt, is_back_in_docking);
		else
			scores[k] = score(10, this_num_steps, this_num_steps, houses[k].initialSumOfDirt - houses[k].sumOfDirt, houses[k].initialSumOfDirt, is_back_in_docking);
		getchar();
	}

	// print scores
	for (int k = 0; k < numOfHouses; k++) {
		if (houses[k].isValidHouse) {
			// in ex1: the output should be only an integer (not the house name)
			cout << "[" << (string)houses[k].houseName << "]\t" << scores[k] << endl;
		}
		else {
			cout << "[" << (string)houses[k].houseName << "]\t" << houses[k].error << endl;
		}
	}

	getchar();
	// free houses
	for (int k = 0; k < numOfHouses; k++) {
		for (int i = 0; i < houses[k].rows; i++)
			delete[] houses[k].matrix[i];
		delete[] houses[k].matrix;
	}
	delete[] houses;
	delete[] scores;

}
int main(int argc, const char* argv[])
{
	// at the end, we wish to print a matrix of scores:
	//		- each row refers to a different house
	//		- each column refers to a different algorithm
	// in ex1 -> there will be only one column
	int numOfHouses;
	House* houses;
	map<string, int> config;

	if (argc > 5)
	{
		cout << MORE_THAN_4_ARGUMENTS << argc << endl;
		return 1;
	}

	if (argc == 1) //both arguments are missing - NEED TO: look for the files in the working directory
	{
		//get the config
		if (handleConfigFile(defaultConfigPath, config) == -1)
		{
			return 1;
		}
		// get number of houses in directory
		numOfHouses = getNumberOfHouses(defaultHousePath);
		if (numOfHouses == -1 || numOfHouses==0)
		{
			std::cout << EMPTY_HOUSE_PATH << std::endl;
			return -1;
		}
		houses = new House[numOfHouses];
		//get the house file
		handleHouseFiles(defaultHousePath, numOfHouses, houses);
	}
	else if (argc == 3) // got only one of the arguments.
	{
		if (!strcmp(argv[1], "-config")) // we got the config path, we'll use the defaultHousePath
		{
			if (handleConfigFile(handleSlash(argv[2]), config) == -1)
			{
				return 1;
			}
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(defaultHousePath);
			if (numOfHouses == -1 || numOfHouses == 0)
			{
				std::cout << EMPTY_HOUSE_PATH << std::endl;
				return -1;
			}
			houses = new House[numOfHouses];
			handleHouseFiles(defaultHousePath, numOfHouses, houses);
		}
		else if (!strcmp(argv[1], "-house_path")) // we got the house path, we'll use the defaultConfigPath
		{
			if (handleConfigFile(handleSlash(defaultConfigPath), config) == -1)
			{
				return 1;
			}
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(argv[2]);
			if (numOfHouses == -1 || numOfHouses == 0)
			{
				std::cout << EMPTY_HOUSE_PATH << std::endl;
				return -1;
			}
			houses = new House[numOfHouses];
			handleHouseFiles(handleSlash(argv[2]), numOfHouses, houses);
		}
		else
		{
			cout << WRONG_ARGUMENTS << endl;
			return 1;
		}
	}
	else if (argc == 5) // got both of the arguments (remember: arguments can be received in any order)
	{
		if (!strcmp(argv[1], "-config") && !strcmp(argv[3], "-house_path"))
		{
			if (handleConfigFile(handleSlash(argv[2]), config) == -1)
			{
				return 1;
			}
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(argv[4]);
			if (numOfHouses == -1 || numOfHouses == 0)
			{
				std::cout << EMPTY_HOUSE_PATH << std::endl;
				return -1;
			}
			houses = new House[numOfHouses];
			handleHouseFiles(handleSlash(argv[4]), numOfHouses, houses);
		}
		else if (!strcmp(argv[1], "-house_path") && !strcmp(argv[3], "-config"))
		{
			if (handleConfigFile(handleSlash(argv[4]), config) == -1)
			{
				return 1;
			}
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(argv[2]);
			if (numOfHouses == -1 || numOfHouses == 0)
			{
				std::cout << EMPTY_HOUSE_PATH << std::endl;
				return -1;
			}
			houses = new House[numOfHouses];
			handleHouseFiles(handleSlash(argv[2]), numOfHouses, houses);
		}
		else
		{
			cout << WRONG_ARGUMENTS << endl;
			return 1;
		}
	}
	else // ERROR: every argument represented by two strings - and we got odd number of arguments
	{
		cout << WRONG_ARGUMENTS_NUM << argc << endl;
		return 1;
	}


	startSimulation(houses, numOfHouses, config);

	

    return 0;
}