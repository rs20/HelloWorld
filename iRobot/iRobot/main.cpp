// Simulation (main) : Defines the entry point for the console application.
//
// RON
#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

// include for sleep
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifndef __HOUSE_H
#define __HOUSE_H
#include "House.h"
#endif

#include "Algorithm.cpp"


#define MAX(a,b) (((a)>(b)) ? (a):(b))

void handleConfigFile(string configPath, map<string,int>& house);
void handleHouseFile(string housePath, House& house);
void printHouseWithRobot(House& house);


string defaultConfigPath = "config/";
string defaultHousePath = "house/";

int main(int argc, const char* argv[])
{
	string line;
	House house;
	map<string, int> config;

	if (argc > 5)
	{
		cout << "Expecting maximum of 4 arguments, instead got " << argc << endl;
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
		cout << "incorrect number of arguments" << argc << endl;
	}

	// just checking too see if it worked (print the house)
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				cout << house.matrix[i][j] << "  ";
			}
			cout << endl;
		}
	}

	// start the game:
	//Sensor sensor(house);
	Sensor *sensor = new Sensor(&house);
	Algorithm alg;
	alg.setSensor(sensor);
	alg.setConfiguration(config);

	Sleep(1);
	cout << "iRobot starts cleaning the house.." << endl;
	printHouseWithRobot(house);

	// simulate the algorithm on the house:
	for (int i = 1; i <= config.at("MaxSteps"); i++) {
		cout << "Robot Battery: " << alg.getCurBattery() << endl;
		Sleep(1000);
		cout << "Step " << i << endl;
		//getchar();
		Direction direction = alg.step();
		if (house.matrix[house.robotRow][house.robotCol] > '0' && house.matrix[house.robotRow][house.robotCol] <= '9') {
			house.matrix[house.robotRow][house.robotCol] = house.matrix[house.robotRow][house.robotCol] - 1;
			house.sumOfDirt--;
		}

		if (house.matrix[house.robotRow][house.robotCol] == 'D') { // charge battery
			cout << "charging battery" << endl;
			alg.chargeBattery();
		}
		else {
			alg.consumeBattery();
		}

		if (direction == Direction::East) {
			house.robotCol++;
		}
		else if (direction == Direction::West) {
			house.robotCol--;
		}
		else if (direction == Direction::South) {
			house.robotRow++;
		}
		else if (direction == Direction::North) {
			house.robotRow--;
		}
		printHouseWithRobot(house);

		alg.madeStep();

		if (house.sumOfDirt == 0 && house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) {
			cout << "Robot wins (cleaned the whole house in the limited time." << endl;
			break;
		}
		if (alg.getCurBattery() <= 0) {
			cout << "Battery's Dead! Game Over." << endl;
			break;
		}
		if (alg.getMovesAvailable() == 0) {
			cout << "Time's up! No more moves." << endl;
			break;
		}
	}

	// score the algorithm on the house
	int score = MAX(0, 2000 
						- 3*(house.initialSumOfDirt - (house.initialSumOfDirt - house.sumOfDirt)) // -3*(sum_dirt_in_house - dirt_collected)
						+ ((house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) ? 50 : -200)); // +50 if back in docking station, -200 if not
	cout << "The score of the algorithm on the house is " << score << "." << endl;

	getchar();

	// free house matrix
	for (int i = 0; i < house.rows; i++)
		delete[] house.matrix[i];
	delete[] house.matrix;

    return 0;
}


static vector<string> split(const string &s, char delimiter)
{
	vector<string> elements;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delimiter))
		elements.push_back(item);
	return elements;
}

static string trim(string& str)
{
	str.erase(0, str.find_first_not_of(' ')); // remove prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1); // remove surfixing spaces
	return str;
}

void handleConfigFile(string configPath, map<string,int> &config)
{
	ifstream myfile(configPath + "config.ini");
	string line;
	
	// update according to split 'sarel - המתרגל' did on last recitation
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			vector<string> tokens = split(line, '=');
			if (tokens.size() != 2)
				continue;
			config[trim(tokens[0])] = stoi(tokens[1]);
		}
		myfile.close();
	}
}


void handleHouseFile(string housePath, House& house)
{
	ifstream myfile(housePath + "simple1.house");
	string line;

	if (myfile.is_open()) {
		getline(myfile, line);
		house.houseName = line;
		getline(myfile, line);
		house.houseDescription = line;
		getline(myfile, line);
		house.rows = stoi(line);
		getline(myfile, line);
		house.cols = stoi(line);

		house.initialSumOfDirt = 0;

		house.matrix = new char*[house.rows];
		for (int i = 0; i < house.rows; i++)
			house.matrix[i] = new char[house.cols];

		for (int i = 0; i < house.rows; i++)
		{
			getline(myfile, line);
			for (int j = 0; j < house.cols; j++)
			{
				house.matrix[i][j] = line[j];
				// found robot's starting point == docking station
				if (line[j] == 'D') {
					house.robotRow = i;
					house.robotCol = j;
					house.dockingRow = i;
					house.dockingCol = j;
				}
				if (line[j] >= '1' && line[j] <= '9')
					house.initialSumOfDirt += (line[j] - '0');
			}
		}

		house.sumOfDirt = house.initialSumOfDirt;

		myfile.close();
	}
	else {
		cout << "Error opening the house file." << endl;
	}
}


void printHouseWithRobot(House& house)
{
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				if (i == house.robotRow && j == house.robotCol)
					cout << 'R' << "  ";
				else
					cout << house.matrix[i][j] << "  ";
			}
			cout << endl;
		}
	}
}