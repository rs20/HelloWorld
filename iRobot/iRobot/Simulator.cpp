// Simulation (main,cpp) : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "simulator.h"


void handleConfigFile(string configPath, map<string,int>& house);
void handleHouseFile(string housePath, House& house);
void printHouseWithRobot(House& house);
int scoreEx1(int position_in_copmetition, int winner_num_steps, int this_num_steps, int dirt_collected, int sum_dirt_in_house, int is_back_in_docking);


int main(int argc, const char* argv[])
{
	string line;
	House house;
	map<string, int> config;
	int score;
	int is_back_in_docking;
	int winner_num_steps;
	int this_num_steps = 0;
	int moreSteps;
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

	// just checking too see if it worked (print the house) - for debug purpose
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
	winner_num_steps = (config.find("MaxSteps"))->second;
	moreSteps = (config.find("MaxSteps"))->second;
	batteryCapacity = (config.find("BatteryCapacity"))->second;;
	curBattery = batteryCapacity;
	batteryConsumptionRate = (config.find("BatteryConsumptionRate"))->second;;
	batteryRechargeRate = (config.find("BatteryRechargeRate"))->second;;

	Sleep(1);
	cout << "iRobot starts cleaning the house.." << endl;
	printHouseWithRobot(house);

	// simulate the algorithm on the house:
	for (int i = 1; i <= config.at("MaxSteps"); i++) {
		cout << "Robot Battery: " << curBattery << endl;
		Sleep(10);
		cout << "Step " << i << endl;
		
		Direction direction = alg.step();
		//cleaning dust if there is any.
		if (house.matrix[house.robotRow][house.robotCol] > '0' && house.matrix[house.robotRow][house.robotCol] <= '9') {
			house.matrix[house.robotRow][house.robotCol] = house.matrix[house.robotRow][house.robotCol] - 1;
			house.sumOfDirt--;
		}

		if (house.matrix[house.robotRow][house.robotCol] == 'D') { // charge battery
			cout << "charging battery" << endl;
			curBattery = MIN(batteryCapacity, curBattery + batteryRechargeRate);
		}
		else {
			curBattery -= batteryConsumptionRate;
		}

		switch (direction)
		{
			case Direction::East:
				house.robotCol++;
				break;
			case Direction::West:
				house.robotCol--;
				break;
			case Direction::South:
				house.robotRow++;
				break;
			case Direction::North:
				house.robotRow--;
				break;
		}

		printHouseWithRobot(house); // for debug purpose

		this_num_steps++;
		moreSteps--;

		if (house.sumOfDirt == 0 && house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) { 
			if (winner_num_steps >= this_num_steps)
			{
				winner_num_steps = this_num_steps;
			}
			cout << "Robot wins (cleaned the whole house in the limited time." << endl; //  for debug purpose
			break;
		}
		if (curBattery <= 0) { 
			cout << BATTERY_DEAD << endl; // for debug purpose
			break;
		}
		if (moreSteps == 0) {
			cout << NO_MORE_MOVES << endl;  // for debug purpose
			break;
		}
	}

	// score the algorithm on the house
	is_back_in_docking = (house.robotRow == house.dockingRow && house.robotCol == house.dockingCol) ? 1 : 0;
	score = scoreEx1(1, winner_num_steps, this_num_steps, house.initialSumOfDirt - house.sumOfDirt, house.initialSumOfDirt, is_back_in_docking);

	cout << "[" << (string)house.houseName << "]\t" << score << endl;

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
	ifstream myfile(configPath + defaultConfigFile);
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
	else {
		cout << ERROR_CONFIG_FILE << endl;
	}
}



void handleHouseFile(string housePath, House& house)
{
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	wstring stemp = stringToWstring(housePath + "*.house");
	HANDLE hFile = FindFirstFile(stemp.c_str(), &fd);
	wstring tempFileName = L"";
	string fileName = "";

	if (INVALID_HANDLE_VALUE != hFile)
	{
		do
		{
			tempFileName = wstring(fd.cFileName);
			fileName = string(tempFileName.begin(), tempFileName.end());
		} while (FindNextFile(hFile, &fd));
		FindClose(hFile);
	}

#endif
	//need to add an option for linux using readdir and #include <dirent.h>

	ifstream myfile(housePath + fileName);
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
		cout << ERROR_HOUSE_FILE << endl;
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

int scoreEx1(int position_in_copmetition, int winner_num_steps, int this_num_steps, int dirt_collected, int sum_dirt_in_house, int is_back_in_docking)
{

	return MAX(0, 2000
		- 50 * (position_in_copmetition - 1)
		- 10 * (winner_num_steps - this_num_steps)
		- 3 * (sum_dirt_in_house - dirt_collected)
		+ ((is_back_in_docking) ? 50 : -200)); // +50 if back in docking station, -200 if not

}