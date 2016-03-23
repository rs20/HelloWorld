// Simulation (main,cpp) : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Simulator.h"
#include "Auxiliary.h"
#include "Score.h"


int main(int argc, const char* argv[])
{
	House house;
	map<string, int> config;
	int score;
	int is_back_in_docking;
	int max_steps;
	int this_num_steps = 0;
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
		//get the house file
		handleHouseFile(defaultHousePath, house);
	}
	else if (argc == 3) // got only one of the arguments.
	{
		if (!strcmp(argv[1], "-config")) // we got the config path, we'll use the defaultHousePath
		{
			handleConfigFile(argv[2], config);
			handleHouseFile(defaultHousePath, house);
		}
		else // we got the house path, we'll use the defaultConfigPath
		{
			handleConfigFile(defaultConfigPath, config);
			handleHouseFile(argv[2], house);
		}
	}
	else if (argc == 5) // got both of the arguments.
	{
		handleConfigFile(argv[2], config);
		handleHouseFile(argv[4], house);
	}
	else // ERROR: every argument represented by two strings - and we got odd number of arguments
	{
		cout << WRONG_ARGUMENTS_NUM << argc << endl;
	}

	// just checking to see if it worked (print the house) - for debug purpose
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				cout << house.matrix[i][j] << "  ";
			}
			cout << endl;
		}
	}

	// start the game:
	Sensor *sensor = new Sensor(&house);
	Algorithm alg;
	alg.setSensor(sensor);
	alg.setConfiguration(config);
	max_steps = (config.find("MaxSteps"))->second;
	batteryCapacity = (config.find("BatteryCapacity"))->second;;
	curBattery = batteryCapacity;
	batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;;
	batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;;

	Sleep(1000);
	cout << "iRobot starts cleaning the house.." << endl;
	printHouseWithRobot(house);

	// simulate the algorithm on the house:
	int i = 0;
	bool goodStep = true;
	while (true) {
		i++;
		cout << "Robot Battery: " << curBattery << endl;
		Sleep(100);
		cout << "Step " << i << endl;
		
		Direction direction = alg.step();

		//cleaning dust if there is any.
		if (house.matrix[house.robotRow][house.robotCol] > '0' && house.matrix[house.robotRow][house.robotCol] <= '9') {
			house.matrix[house.robotRow][house.robotCol] = house.matrix[house.robotRow][house.robotCol] - 1;
			house.sumOfDirt--;
		}

		switch (direction)
		{
			case Direction::East:
				house.robotCol++;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::West:
				house.robotCol--;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::South:
				house.robotRow++;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::North:
				house.robotRow--;
				curBattery -= batteryConsumptionRate;
				break;
			case Direction::Stay:
				// charge battery only if the robot stays in the docking station
				if (house.matrix[house.robotRow][house.robotCol] == 'D') {
					curBattery = MIN(batteryCapacity, curBattery + batteryRechargeRate);
				}
				else {
					curBattery -= batteryConsumptionRate;
				}
		}
		if (house.matrix[house.robotRow][house.robotCol] == 'W') { // walked into a wall -> stop the algorithm immediately. its score will be zero
			goodStep = false;
			cout << INTO_WALL << endl; // for debug purpose
			break;
		}

		printHouseWithRobot(house); // for debug purpose

		this_num_steps++;

		if (house.sumOfDirt == 0 && house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) { 
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
	is_back_in_docking = (house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) ? 1 : 0;
	score = scoreEx1(1, this_num_steps, this_num_steps, house.initialSumOfDirt - house.sumOfDirt, house.initialSumOfDirt, is_back_in_docking);
	if (goodStep == false) // if walked into a wall, score=0
		score = 0;

	cout << "[" << (string)house.houseName << "]\t" << score << endl;

	getchar();

	// free house matrix
	for (int i = 0; i < house.rows; i++)
		delete[] house.matrix[i];
	delete[] house.matrix;

    return 0;
}