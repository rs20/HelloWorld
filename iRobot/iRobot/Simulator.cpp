// Simulation (main,cpp) : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Simulator.h"
#include "Auxiliary.h"
#include "Score.h"


int main(int argc, const char* argv[])
{
	// at the end, we wish to print a matrix of scores:
	//		- each row refers to a different house
	//		- each column refers to a different algorithm
	// in ex1 -> there will be only one column
	int numOfHouses;
	House* houses;
	int* scores;
	map<string, int> config;
	int is_back_in_docking;
	int max_steps;
	int this_num_steps;
	int batteryCapacity;
	int curBattery;
	int batteryConsumptionRate;
	int batteryRechargeRate;

	if (argc > 5)
	{
		cout << MORE_THAN_4_ARGUMENTS << argc << endl;
		return 1;
	}

	if (argc == 1) //both arguments are missing - NEED TO: look for the files in the working directory
	{
		//get the config
		handleConfigFile(defaultConfigPath, config);
		// get number of houses in directory
		numOfHouses = getNumberOfHouses(defaultHousePath);
		houses = new House[numOfHouses];
		//get the house file
		handleHouseFiles(defaultHousePath, numOfHouses, houses);
	}
	else if (argc == 3) // got only one of the arguments.
	{
		if (!strcmp(argv[1], "-config")) // we got the config path, we'll use the defaultHousePath
		{
			handleConfigFile(argv[2], config);
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(defaultHousePath);
			houses = new House[numOfHouses];
			handleHouseFiles(defaultHousePath, numOfHouses, houses);
		}
		else // we got the house path, we'll use the defaultConfigPath
		{
			handleConfigFile(defaultConfigPath, config);
			// get number of houses in directory
			numOfHouses = getNumberOfHouses(argv[2]);
			houses = new House[numOfHouses];
			handleHouseFiles(argv[2], numOfHouses, houses);
		}
	}
	else if (argc == 5) // got both of the arguments.
	{
		handleConfigFile(argv[2], config);
		// get number of houses in directory
		numOfHouses = getNumberOfHouses(argv[2]);
		houses = new House[numOfHouses];
		handleHouseFiles(argv[4], numOfHouses, houses);
	}
	else // ERROR: every argument represented by two strings - and we got odd number of arguments
	{
		cout << WRONG_ARGUMENTS_NUM << argc << endl;
		return 1;
	}


	// iterate over all houses
	scores = new int[numOfHouses];
	for (int k = 0; k < numOfHouses; k++)
	{
		// just checking to see if it worked (print the house) - for debug purpose
		if (houses[k].matrix != NULL) {
			for (int i = 0; i < houses[k].rows; i++) {
				for (int j = 0; j < houses[k].cols; j++) {
					cout << houses[k].matrix[i][j] << "  ";
				}
				cout << endl;
			}
		}

		// start the game:
		Sensor *sensor = new Sensor(&houses[k]);
		Algorithm alg;
		alg.setSensor(sensor);
		alg.setConfiguration(config);
		max_steps = (config.find("MaxSteps"))->second;
		batteryCapacity = (config.find("BatteryCapacity"))->second;;
		curBattery = batteryCapacity;
		batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;
		batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;
		this_num_steps = 0;

		Sleep(1000);
		cout << "iRobot starts cleaning the house.." << endl;
		printHouseWithRobot(houses[k]);

		// simulate the algorithm on the house:
		int i = 0;
		bool goodStep = true;
		while (true) {
			i++;
			cout << "Robot Battery: " << curBattery << endl;
			Sleep(1);
			cout << "Step " << i << endl;

			Direction direction = alg.step();

			//cleaning dust if there is any.
			if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] > '0' && houses[k].matrix[houses[k].robotRow][houses[k].robotCol] <= '9') {
				houses[k].matrix[houses[k].robotRow][houses[k].robotCol] = houses[k].matrix[houses[k].robotRow][houses[k].robotCol] - 1;
				houses[k].sumOfDirt--;
			}

			switch (direction)
			{
			case Direction::East:
				houses[k].robotCol++;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::West:
				houses[k].robotCol--;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::South:
				houses[k].robotRow++;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::North:
				houses[k].robotRow--;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::Stay:
				// charge battery only if the robot stays in the docking station
				if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] == 'D') {
					curBattery = MIN(batteryCapacity, curBattery + batteryRechargeRate);
					//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~` CHARGING BATTERY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
				}
				else {
					curBattery -= batteryConsumptionRate;
				}
			}
			if (houses[k].matrix[houses[k].robotRow][houses[k].robotCol] == 'W') { // walked into a wall -> stop the algorithm immediately. its score will be zero
				goodStep = false;
				cout << INTO_WALL << endl; // for debug purpose
				break;
			}

			//printHouseWithRobot(houses[k]); // for debug purpose

			this_num_steps++;

			if (houses[k].sumOfDirt == 0 && houses[k].robotRow == houses[k].dockingRow && houses[k].robotCol == houses[k].dockingCol) {
				cout << "Robot wins (cleaned the whole house in the limited time)." << endl; //  for debug purpose
				break;
			}
			if (curBattery <= 0) {
				cout << BATTERY_DEAD << endl; // for debug purpose
				break;
			}
			if (this_num_steps == max_steps) {
				cout << NO_MORE_MOVES << endl;  // for debug purpose
				break;
			}
		}

		// score the algorithm on the house
		is_back_in_docking = (houses[k].robotRow == houses[k].dockingRow && houses[k].robotCol == houses[k].dockingCol) ? 1 : 0;
		scores[k] = scoreEx1(1, this_num_steps, this_num_steps, houses[k].initialSumOfDirt - houses[k].sumOfDirt, houses[k].initialSumOfDirt, is_back_in_docking);
		if (goodStep == false) // if walked into a wall, score=0
			scores[k] = 0;

		// free sensor
		delete sensor;
		getchar();
	}

	// print scores
	for (int k = 0; k < numOfHouses; k++)
		cout << "[" << (string)houses[k].houseName << "]\t" << scores[k] << endl;
	getchar();
	// free houses
	for (int k = 0; k < numOfHouses; k++) {
		for (int i = 0; i < houses[k].rows; i++)
			delete[] houses[k].matrix[i];
		delete[] houses[k].matrix;
	}
	delete[] houses;
	delete[] scores;

    return 0;
}