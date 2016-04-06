#include "stdafx.h"
#include "Auxiliary.h"
#include <stdlib.h>
#include <string>

// make sure the path is ending with "/" (except from the empty path)
std::string handleSlash(const char* path)
{
	if (!(strcmp(path, "")))
	{
		return path;
	}

	size_t stringLength = strlen(path);
	if (path[stringLength - 1] == '/')
	{
		return path;
	}

	char* newPath = strcat((char*)path, "/");
	return newPath;
}


std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> elements;
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delimiter))
		elements.push_back(item);
	return elements;
}

std::string trim(std::string& str)
{
	str.erase(0, str.find_first_not_of(' ')); // remove prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1); // remove surfixing spaces
	return str;
}

// return -1 for exit + no usage message needed
// return -2 for exit + usage message needed
int handleConfigFile(std::string configPath, std::map<std::string, int> &config)
{
	std::string fullFileName = configPath + defaultConfigFile;
	std::ifstream myfile(fullFileName.c_str());
	std::string line;
	// to check if all values were in the file
	std::map<std::string, bool> values;
	values["MaxStepsAfterWinner"] = false;
	values["BatteryCapacity"] = false;
	values["BatteryConsumptionRate"] = false;
	values["BatteryRechargeRate"] = false;

	// check if file exists - if not, return
	if (!myfile.good())
		return -2;

	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			std::vector<std::string> tokens = split(line, '=');
			if (tokens.size() != 2)
				continue;
			config[trim(tokens[0])] = atoi(tokens[1].c_str());
		}
		myfile.close();
	}
	else 
	{
		std::cout << ERROR_CONFIG_FILE1 << configPath << ERROR_CONFIG_FILE2 << std::endl;
		return -1;
	}

	int missing = 4;
	for (std::map<std::string, int>::iterator it = config.begin(); it != config.end(); it++) {
		if (!strcmp((it->first).c_str(), "MaxStepsAfterWinner")) {
			values["MaxStepsAfterWinner"] = true;
			missing--;
		}
		else if (!strcmp((it->first).c_str(), "BatteryCapacity")) {
			values["BatteryCapacity"] = true;
			missing--;
		}
		else if (!strcmp((it->first).c_str(), "BatteryConsumptionRate")) {
			values["BatteryConsumptionRate"] = true;
			missing--;
		}
		else if (!strcmp((it->first).c_str(), "BatteryRechargeRate")) {
			values["BatteryRechargeRate"] = true;
			missing--;
		}
		// else: ignore as specified in the pdf
	}

	if (missing > 0) {
		std::cout << "config.ini missing " << missing << " parameter(s): ";
		bool first = true;
		for (std::map<std::string, bool>::iterator it = values.begin(); it != values.end(); it++) {
			if (values[it->first] == false) {
				if (first) {
					std::cout << it->first;
					first = false;
				}
				else {
					std::cout << ", " << it->first;
				}
			}
		}
		std::cout << std::endl;
		return -1;
	}

	return 0;
}

#ifdef _WIN32
std::wstring stringToWstring(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
#endif

// return -1 for error / 0 for ok
int handleHouseFiles(std::string housePath, int numOfHouses, House* houses)
{
	std::string* fileNames = new std::string[numOfHouses];
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	std::wstring stemp = stringToWstring(housePath + "*.house");
	HANDLE hFile = FindFirstFile(stemp.c_str(), &fd);
	std::wstring tempFileName = L"";
	std::string fileName = "";

	// we read the houses in ascii order (Simple1.house comes before Simple2.house etc)
	int i = -1;
	if (INVALID_HANDLE_VALUE != hFile)
	{
		do
		{
			i++;
			tempFileName = std::wstring(fd.cFileName);
			fileNames[i] = std::string(tempFileName.begin(), tempFileName.end());
		} while (FindNextFile(hFile, &fd));
		FindClose(hFile);
	}
#else
	DIR *pDIR;
	struct dirent *entry;
	std::string temp = "";
	int i = 0;
	if ((pDIR = opendir(housePath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				if (strlen(entry->d_name) > 6) // name of file is "X.house" so it should be > 6 
				{
					temp = entry->d_name;
					temp = temp.substr(strlen(entry->d_name) - 6, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".house"))
					{
						fileNames[i] =  entry->d_name;
						i++;
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		printf("%s\n", "Error in house path");
		return;
	}
#endif

	for (int k = 0; k < numOfHouses; k++)
	{
		int numOfDockingStations = 0;
		houses[k].isValidHouse = true;
		std::string fullFileName = housePath + fileNames[k];
		houses[k].houseFileName = fullFileName;
		std::ifstream myfile(fullFileName.c_str());
		std::string line;

		if (myfile.is_open()) {
			getline(myfile, line);
			houses[k].houseDescription = line;
			getline(myfile, line);
			if (atoi(line.c_str()) < 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 2 in house file shall be a positive number, found: " + atoi(line.c_str());
				continue;
			}
			houses[k].maxSteps = atoi(line.c_str());
			getline(myfile, line);
			if (atoi(line.c_str()) < 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 3 in house file shall be a positive number, found: " + atoi(line.c_str());
				continue;
			}
			houses[k].rows = atoi(line.c_str());
			getline(myfile, line);
			if (atoi(line.c_str()) < 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 4 in house file shall be a positive number, found: " + atoi(line.c_str());
				continue;
			}
			houses[k].cols = atoi(line.c_str());

			houses[k].initialSumOfDirt = 0;

			// initialize empty matrix of spaces of size rows X cols
			houses[k].matrix = new char*[houses[k].rows];
			for (int i = 0; i < houses[k].rows; i++) {
				houses[k].matrix[i] = new char[houses[k].cols];
				for (int j = 0; j < houses[k].cols; j++)
					houses[k].matrix[i][j] = ' ';
			}

			// start reading the house matrix
			getline(myfile, line);
			for (int i = 0; i < houses[k].rows && myfile; i++)
			{
				for (int j = 0; j < houses[k].cols && j < ((int)line.length()); j++)
				{
					houses[k].matrix[i][j] = line[j];
					// found robot's starting point == docking station
					if (line[j] == 'D') {
						numOfDockingStations++;
						houses[k].robotRow = i;
						houses[k].robotCol = j;
						houses[k].dockingRow = i;
						houses[k].dockingCol = j;
					}
					// treat any unrecognized character as ' '
					else if (line[j] != 'W' && line[j] != ' ' && (line[j] < '0' || line[j] > '9'))
						houses[k].matrix[i][j] = ' ';

					if (line[j] >= '1' && line[j] <= '9')
						houses[k].initialSumOfDirt += (line[j] - '0');
				}
				getline(myfile, line);
			}
			if (numOfDockingStations == 0)
			{
				houses[k].isValidHouse = false;
				houses[k].error = ERROR_NO_DOCKING_STATIONS;
				continue;
			}

			// reaches here if there is at least one docking station
			// filling the house walls
			// fill most left and most right sides with walls
			for (int i = 0; i < houses[k].rows; i++)
			{
				if (houses[k].matrix[i][0] == 'D') {
					numOfDockingStations--;
				}
				houses[k].matrix[i][0] = 'W';
				if (houses[k].matrix[i][houses[k].cols - 1] == 'D') {
					numOfDockingStations--;
				}
				houses[k].matrix[i][houses[k].cols -1] = 'W';
			}
			// fill most up and most down sides with walls
			for (int j = 0; j < houses[k].cols; j++)
			{
				if (houses[k].matrix[0][j] == 'D') {
					numOfDockingStations--;
				}
				houses[k].matrix[0][j] = 'W';
				if (houses[k].matrix[houses[k].rows - 1][j] == 'D') {
					numOfDockingStations--;
				}
				houses[k].matrix[houses[k].rows - 1][j] = 'W';
			}

			// check if number of dokcing stations is different than 1
			if (numOfDockingStations == 0) {
				houses[k].isValidHouse = false;
				houses[k].error = ERROR_NO_DOCKING_STATIONS;
				continue;
			}
			else if (numOfDockingStations > 1) {
				houses[k].isValidHouse = false;
				houses[k].error = ERROR_TOO_MANY_DOCKING_STATIONS;
				continue;
			}

			houses[k].sumOfDirt = houses[k].initialSumOfDirt;
			myfile.close();
		}
		else {
			houses[k].isValidHouse = false;
			houses[k].error = ERROR_OPEN_HOUSE_FILE;
		}
	}

	bool allMalformed = true;
	for (int i = 0; i < numOfHouses; i++) {
		if (houses[i].isValidHouse == true)
			allMalformed = false;
	}
	if (!allMalformed)
		return 0;

	std::cout << "All house files in target folder " << housePath << " cannot be opened or are invalid:" << std::endl;
	for (int i = 0; i < numOfHouses; i++) {
		std::cout << houses[i].houseFileName << ":" << houses[i].error << std::endl;
	}
	return -1;
}


// returns number of house files in housePath directory
int getNumberOfHouses(std::string housePath)
{
	int numOfHouses = 0;
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	std::wstring stemp = stringToWstring(housePath + "*.house");
	HANDLE hFile = FindFirstFile(stemp.c_str(), &fd);
	std::wstring tempFileName = L"";
	std::string fileName = "";

	if (INVALID_HANDLE_VALUE != hFile)
	{
		do
		{
			tempFileName = std::wstring(fd.cFileName);
			fileName = std::string(tempFileName.begin(), tempFileName.end());
			numOfHouses++;
		} while (FindNextFile(hFile, &fd));
		FindClose(hFile);
	}
	else
	{
		// TODO: figure out if should print usage and exit?
		return -1;
	}

#else
	DIR *pDIR;
	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(housePath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
			{
				if (strlen(entry->d_name) > 6) // name of file is "X.house" so it should be > 6 
				{
					temp = entry->d_name;
					temp = temp.substr(strlen(entry->d_name) - 6, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".house"))
					{
						numOfHouses++;
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		// TODO: figure out if should print usage and exit?
		return -1;
	}
#endif
	return numOfHouses;
}



int handleAlgorithmFiles(std::string algorithmPath, int numOfAlgorithms)
{
	return 0;
}


// returns number of house files in housePath directory
int getNumberOfAlgorithms(std::string algorithmPath)
{
	int numOfAlgorithms = 0;
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	std::wstring stemp = stringToWstring(algorithmPath + "*.so");
	HANDLE hFile = FindFirstFile(stemp.c_str(), &fd);
	std::wstring tempFileName = L"";
	std::string fileName = "";

	if (INVALID_HANDLE_VALUE != hFile)
	{
		do
		{
			tempFileName = std::wstring(fd.cFileName);
			fileName = std::string(tempFileName.begin(), tempFileName.end());
			numOfAlgorithms++;
		} while (FindNextFile(hFile, &fd));
		FindClose(hFile);
	}
	else
	{
		// TODO: figure out if should print usage and exit?
		return -1;
	}

#else
	DIR *pDIR;
	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(housePath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				if (strlen(entry->d_name) > 3) // name of file is "X.so" so it should be > 3 
				{
					temp = entry->d_name;
					temp = temp.substr(strlen(entry->d_name) - 3, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".so"))
					{
						numOfAlgorithms++;
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		// TODO: figure out if should print usage and exit?
		return -1;
	}
#endif
	return numOfAlgorithms;
}


void printHouseWithRobot(House& house)
{
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				if (i == house.robotRow && j == house.robotCol)
					std::cout << 'R' << "";
				else
					std::cout << house.matrix[i][j] << "";
			}
			std::cout << std::endl;
		}
	}
}


void usageMessage(std::string configPath, std::string housePath, std::string algorithmPath)
{
	std::cout << "Usage: simulator";
	if (!configPath.empty())
		std::cout << " -config <" << configPath << ">]";
	if (!housePath.empty())
		std::cout << " -house_path <" << housePath << ">]";
	if (!algorithmPath.empty())
		std::cout << " -algorithm_path <" << algorithmPath << ">]";
	std::cout << std::endl;
}


void copyHouse(House& dst, House& src)
{
	dst.houseFileName = src.houseFileName;
	dst.houseDescription = src.houseDescription;
	dst.rows = src.rows;
	dst.cols = src.cols;
	dst.matrix = new char*[src.rows];
	for (int i = 0; i < src.rows; i++) {
		dst.matrix[i] = new char[src.cols];
		for (int j = 0; j < src.cols; j++)
			dst.matrix[i][j] = src.matrix[i][j];
	}
	dst.robotRow = src.robotRow;
	dst.robotCol = src.robotCol;
	dst.dockingRow = src.dockingRow;
	dst.dockingCol = src.dockingCol;
	dst.initialSumOfDirt = src.initialSumOfDirt;
	dst.sumOfDirt = src.sumOfDirt;
}