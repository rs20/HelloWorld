#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string defaultConfigPath = "config/";
string defaultHousePath  = "house/";

typedef struct House
{
	string houseName;
	string houseDescription;
	char** matrix;
	int row;
	int col;
}House;


void handleConfigFile(string configPath, House& house)
{
	ifstream myfile(configPath);

	myfile.close();
}

void handleHouseFile(string housePath, House& house)
{
	// NEED TO UPDATE !!!
	//here we need to get all the house from the path .... for example I'm gonna open only one for now

	ifstream myfile(defaultHousePath + "exp.house");
	string line;
	string b;
	if (myfile.is_open())
	{
		getline(myfile, line);
		house.houseName = line;
		getline(myfile, line);
		house.houseDescription = line;
		getline(myfile, line);
		house.row = stoi(line);
		getline(myfile, line);
		house.col = stoi(line);

		house.matrix = new char*[house.row];
		for (int i = 0; i < house.row; i++)
			house.matrix[i] = new char[house.col];

		for (int i = 0; i < house.row; i++)
		{
			getline(myfile, line);
			for (int j = 0; j < line.length(); j++)
			{
				house.matrix[i][j] = line[j];
			}
		}


		myfile.close();

	}
}

int main(int argc, const char* argv[])
{
	string line;
	House house;

	if (argc > 5)
	{
		cout << "Expecting maximum of 4 arguments, instead got " << argc  << endl;
		return 1;
	}



	if (argc == 1) //both arguments are missing - NEED TO: look for the files in the working directory
	{
		//get the config
		handleConfigFile(defaultConfigPath, house);
		//get the house file
		handleHouseFile(defaultHousePath, house);


	}
	else if (argc == 3) // got only one of the arguments.
	{
		if (!strcmp(argv[1],"-config")) // we got the config path, we'll use the defaultHousePath
		{
			handleConfigFile(argv[2], house);
			handleHouseFile(defaultHousePath, house);
		}
		else // we got the house path, we'll use the defaultConfigPath
		{
			handleConfigFile(defaultConfigPath, house);
			handleHouseFile(argv[2], house);
		}
	}
	else if (argc == 5) // got both of the arguments.
	{
		handleConfigFile(argv[2], house);
		handleHouseFile(argv[4], house);
	}
	else // ERROR: every argument represented by two strings - and we got odd number of arguments
	{
		cout << "incorrect number of arguments" << argc << endl;
	}


	
	// just checking too see if it worked
	if (house.matrix != NULL)
	{
		for (int i = 0; i < house.row; i++)
		{
			for (int j = 0; j < house.col; j++)
			{
				cout << house.matrix[i][j] << "   ";
			}
			cout << endl;
		}
	}

	//adding a comment to see if git working fine

	getchar();
	return 0;
}