//#include "stdafx.h"

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

// return 0 for ok
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
	int errorInFile = 0; // 0 : no error , -1 : here is an error.
	char* end;
	int numBadValues = 0;

	// check if file exists - if not, return
	struct stat st;
	if (stat(fullFileName.c_str(), &st) == -1)
		return -2;
	if (!myfile.good()) {
		std::cout << ERROR_CONFIG_FILE1 << configPath << ERROR_CONFIG_FILE2 << std::endl;
		return -1;
	}

	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			std::vector<std::string> tokens = split(line, '=');
			if (tokens.size() != 2)
				continue;
			if (tokens[1].empty() || (!isdigit((tokens[1])[0]))) // if the string doesn't represent a number, define the number to be -1;
			{
				config[trim(tokens[0])] = -1;
				continue;
			}
			strtol(tokens[1].c_str(), &end, 10); // // if the string doesn't represent a number, define the number to be -1;
			if (*end != 0)
			{
				config[trim(tokens[0])] = -1;
				continue;
			}
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
		errorInFile = - 1;
	}

	//count the number of bad values.
	for (std::map<std::string, int>::iterator it = config.begin(); it != config.end(); it++) {
		if (it->second < 0)	{
			numBadValues++;
		}
	}

	//print all the bad values.
	if (numBadValues>0)
	{
		errorInFile = -1;
		std::cout << ERROR_CONFIG_FILE_BAD_VALUE << numBadValues << " parameter(s): ";
		for (std::map<std::string, int>::iterator it = config.begin(); it != config.end(); it++) {
			if (it->second < 0)	{
				if (numBadValues>1)
					std::cout << it->first << ", ";
				else
					std::cout << it->first << endl;
				numBadValues--;
			}
		}

	}

	return errorInFile;
}

// return 0 for ok / -1 for .... / -2 for
int handleScoreFile(std::string scorePath, void* hndl, int (**score_function)(const map<string, int>& score_params))
{
	std::string fullFileName = scorePath + defaultScoreFile;
	bool found = false;
	DIR *pDIR;
	struct stat st;
	if (stat(scorePath.c_str(), &st) == -1) // error (we know the directory exists)
	{
		return -2;
	}

	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(scorePath.empty() ? "." : scorePath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				if (strlen(entry->d_name) > 3) // name of file is "X.so" so it should be > 3
				{
					temp = entry->d_name;
					string temp_name = temp.substr(0, strlen(entry->d_name) - 3);
					temp = temp.substr(strlen(entry->d_name) - 3, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".so"))
					{
						if (!strcmp(temp_name.c_str(), "score_formula")) {
							found = true;
							break;
						}
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		return -2;
	}

	if (!found)
	{
		return -2;
	}


	//found the score formula, try to load it dynamically:

	hndl = dlopen(fullFileName.c_str(), RTLD_NOW);;
	if (hndl == NULL)
	{
		std::cout << "score_formula.so exist in " << scorePath << " but cannot be opened or is not a valid .so"  << endl;
		return -1;
	}

	dlerror();    /* Clear any existing error */

	*(void **)(score_function) = dlsym(hndl, "calc_score");
	if (dlerror() != NULL)  
	{
		std::cout << ERROR_NOT_VALID_SCORE << endl;
		return -1;
	}

	return 0;
}

// we call this method after checking in getNumberOfHouses that housePath exists (as a directory) and contains > 0 houses
// return 0 for ok / -1 for error + should print usage / -2 for error + return
int handleHouseFiles(std::string housePath, int numOfHouses, House* houses)
{
	//std::string* fileNames = new std::string[numOfHouses];
	//unique_ptr<string[]> fileNames = make_unique<string[]>(numOfHouses);
	vector<string> fileNames;
	// linux code
	DIR *pDIR;
	struct stat st;
	if (!(housePath.empty())) {
		if (stat(housePath.c_str(), &st) == -1) // error (we know the directory exists)
		{
			return -1;
		}
	}
	struct dirent *entry;
	std::string temp = "";
	//int i = 0;
	if ((pDIR = opendir(housePath.empty() ? "." : housePath.c_str())))
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
						fileNames.push_back(entry->d_name);
						//fileNames[i] =  entry->d_name;
						//i++;
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		// no need to delete when working with smart pointers
		//delete[] fileNames;
		return -1;
	}
	//sort(fileNames, fileNames + numOfHouses);
	sort(fileNames.begin(), fileNames.end());
	for (int k = 0; k < numOfHouses; k++)
	{
		int numOfDockingStations = 0;
		houses[k].isValidHouse = true;
		std::string fullFileName = housePath + fileNames[k];
		houses[k].houseFileName = fullFileName;
		std::ifstream myfile(fullFileName.c_str());
		std::string line;

		houses[k].ifToFree = false;
		if (myfile.is_open()) {
			getline(myfile, line);
			houses[k].houseDescription = line;
			getline(myfile, line);
			if (atoi(line.c_str()) < 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 2 in house file shall be a positive number, found: " + line;
				continue;
			}
			houses[k].maxSteps = atoi(line.c_str());
			getline(myfile, line);
			if (atoi(line.c_str()) <= 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 3 in house file shall be a positive number, found: " + line;
				continue;
			}
			houses[k].rows = atoi(line.c_str());
			getline(myfile, line);
			if (atoi(line.c_str()) <= 0) {
				houses[k].isValidHouse = false;
				houses[k].error = "line number 4 in house file shall be a positive number, found: " + line;
				continue;
			}
			houses[k].cols = atoi(line.c_str());

			houses[k].initialSumOfDirt = 0;

			// initialize empty matrix of spaces of size rows X cols
			//houses[k].matrix = new char*[houses[k].rows];
			houses[k].matrix = make_unique<unique_ptr<char[]>[]>(houses[k].rows);
			for (int i = 0; i < houses[k].rows; i++) {
				//houses[k].matrix[i] = new char[houses[k].cols];
				houses[k].matrix[i] = make_unique<char[]>(houses[k].cols);
				for (int j = 0; j < houses[k].cols; j++)
					houses[k].matrix[i][j] = ' ';
			}
			houses[k].ifToFree = true;

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
						houses[k].robot = { i, j };
						houses[k].docking = { i, j };
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

			// once again, find the new docking station
			for (int i = 0; i < houses[k].rows; i++) {
				for (int j = 0; j < houses[k].cols; j++) {
					if (houses[k].matrix[i][j] == 'D') {
						houses[k].robot = { i, j };
						houses[k].docking = { i, j };
					}
				}
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
	if (!allMalformed) {
		// no need to free with smart pointers
		//delete[] fileNames;
		return 0;
	}

	// linux code
	string fpath = housePath.empty()? "." : housePath;
	char* rpath = realpath(fpath.c_str(), NULL);
	if (rpath != NULL)
		fpath = string(rpath);
	std::cout << "All house files in target folder " << "'" << fpath << "'" << " cannot be opened or are invalid:" << std::endl;
	for (int i = 0; i < numOfHouses; i++) {
		std::cout << houses[i].houseFileName << ": " << houses[i].error << std::endl;
	}
	// no need to free with smart pointers
	//delete[] fileNames;

	return -2;
}


// returns number of house files in housePath directory
// return -2 if directory does not exist
// return -1 if exists but defected / error occured
// return 0 if exists but no houses inside
int getNumberOfHouses(std::string housePath)
{
	int numOfHouses = 0;
	// linux code
	DIR *pDIR;
	struct stat st;
	if (!(housePath.empty())) {
		if (stat(housePath.c_str(), &st) == -1) // DIR doesn't exist or error
		{
			if (errno == ENOENT) { // does not exists
				return -2;
			}
			else { // error opening
				return -1;
			}
		}
	}
	// reaches here if directory exists
	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(housePath.empty() ? "." : housePath.c_str())))
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
		return -1; // error
	}
	return numOfHouses;
}


// we call this method after checking in getNumberOfPotentialAlgorithms that algorithmPath exists (as a directory) and contains > 0 algorithms
// return 0 for ok / -1 for error + should print usage / -2 for error + return
int handleAlgorithmFiles(std::string algorithmPath, int numOfPotentialAlgorithms, AlgorithmRegistrar& algorithms)
{
	int i=0;
	//std::string* fileNames = new std::string[numOfPotentialAlgorithms];
	//unique_ptr<string[]> fileNames = make_unique<string[]>(numOfPotentialAlgorithms);
	vector<string> fileNames;
	bool anyValidAlgorithm = false;
	// linux code
	DIR *pDIR;
	struct stat st;
	if (!(algorithmPath.empty())) {
		if (stat(algorithmPath.c_str(), &st) == -1) // error (we know the directory exists)
		{
			return -1;
		}
	}
	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(algorithmPath.empty() ? "." : algorithmPath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				if (strlen(entry->d_name) > 3) // name of file is "X.so" so it should be > 3
				{
					temp = entry->d_name;
					string temp_name = temp.substr(0, strlen(entry->d_name) - 3);
					temp = temp.substr(strlen(entry->d_name) - 3, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".so"))
					{
						//fileNames[i] = entry->d_name;
						//i++;
						// do not load 'score_formula.so'
						if (strcmp(temp_name.c_str(), "score_formula")) {
							fileNames.push_back(entry->d_name);
						}
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		// no need to delete when working with smart pointers
		// delete[] fileNames;
		return -1;
	}
	// sort the algorithms lexicographically
	//sort(fileNames, fileNames + numOfPotentialAlgorithms);
	sort(fileNames.begin(), fileNames.end());
	for (i = 0; i < numOfPotentialAlgorithms; i++)
	{
		string relative = "./" + algorithmPath;
		int result = algorithms.loadAlgorithm(algorithmPath.empty() ? relative : algorithmPath, fileNames[i]);
		// add to registrar errors list
		if (result != AlgorithmRegistrar::ALGORITHM_REGISTERED_SUCCESSFULY) {
			std::string err = fileNames[i] + ": ";
			if (result == AlgorithmRegistrar::FILE_CANNOT_BE_LOADED)
				err += NOT_VALID_SO;
			else
				err += NOT_VALID_ALGORITHM;
			algorithms.addErrorToList(err);
		}
		else
			anyValidAlgorithm = true;
	}
	// no need of delete when working with smart pointers
	//delete[] fileNames;

	if (anyValidAlgorithm)
	{
		return 0;
	}
	else
	{
		string fpath = algorithmPath.empty()? "." : algorithmPath;
		char* rpath = realpath(fpath.c_str(), NULL);
		if (rpath != NULL)
			fpath = string(rpath);
		std::cout << "All algorithm files in target folder " << "'" << fpath << "'" << " cannot be opened or are invalid:" << std::endl;
		std::list<std::string> errors = algorithms.getErrorsList();
		for (std::list<std::string>::iterator it = errors.begin(); it != errors.end(); ++it) {
			std::cout << *it << std::endl;
		}
		return -2;
	}
}

// returns number of algorithm files in algorithmPath directory
// return -2 if directory does not exist
// return -1 if exists but defected / error occured
// return 0 if exists but no algorithms inside
int getNumberOfPotentialAlgorithms(std::string algorithmPath)
{
	int numOfAlgorithms = 0;
	// linux code
	DIR *pDIR;
	struct stat st;
	if (!(algorithmPath.empty())) {
		if (stat(algorithmPath.c_str(), &st) == -1) // DIR doesn't exist or error
		{
			if (errno == ENOENT) { // does not exists
				return -2;
			}
			else { // error opening
				return -1;
			}
		}
	}
	// reaches here if directory exists
	struct dirent *entry;
	std::string temp = "";
	if ((pDIR = opendir(algorithmPath.empty() ? "." : algorithmPath.c_str())))
	{
		while ((entry = readdir(pDIR)))
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				if (strlen(entry->d_name) > 3) // name of file is "X.so" so it should be > 3 
				{
					temp = entry->d_name;
					string temp_name = temp.substr(0, strlen(entry->d_name) - 3);
					temp = temp.substr(strlen(entry->d_name) - 3, strlen(entry->d_name) - 1);
					if (!strcmp(temp.c_str(), ".so"))
					{
						// do not load 'score_formula.so'
						if (strcmp(temp_name.c_str(), "score_formula")) {
							numOfAlgorithms++;
						}
					}
				}

			}
		}
		closedir(pDIR);
	}
	else
	{
		return -1; // error
	}
	return numOfAlgorithms;
}

/*
void printHouseWithRobot(House& house)
{
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				if (i == house.robot.row && j == house.robot.col)
					std::cout << 'R' << "";
				else
					std::cout << house.matrix[i][j] << "";
			}
			std::cout << std::endl;
		}
	}
}
*/

// just for debugging
// print size according to number of rows
void printHouseWithRobot(House& house)
{
	std::string space = " ";
	if (house.rows > 60 || house.cols > 40)
		space = "";
	if (house.matrix != NULL) {
		for (int i = 0; i < house.rows; i++) {
			for (int j = 0; j < house.cols; j++) {
				if (i == house.robot.row && j == house.robot.col)
					std::cout << 'R' << space;
				else
					std::cout << house.matrix[i][j] << space;
			}
			std::cout << std::endl;
		}
	}
}

void copyHouse(House& dst, House& src)
{
	dst.houseFileName = src.houseFileName;
	dst.houseDescription = src.houseDescription;
	dst.rows = src.rows;
	dst.cols = src.cols;
	//dst.matrix = new char*[src.rows];
	dst.matrix = make_unique<unique_ptr<char[]>[]>(src.rows);
	for (int i = 0; i < src.rows; i++) {
		//dst.matrix[i] = new char[src.cols];
		dst.matrix[i] = make_unique<char[]>(src.cols);
		for (int j = 0; j < src.cols; j++)
			dst.matrix[i][j] = src.matrix[i][j];
	}
	dst.robot = src.robot;
	dst.docking = src.docking;
	dst.initialSumOfDirt = src.initialSumOfDirt;
	dst.sumOfDirt = src.sumOfDirt;
}

/*
// no need when working with smart pointers
void freeHouses(House* houses, int numOfHouses)
{
	// free houses
	for (int k = 0; k < numOfHouses; k++) {
		if (houses[k].ifToFree) {
			for (int i = 0; i < houses[k].rows; i++)
				delete[] houses[k].matrix[i];
			delete[] houses[k].matrix;
		}
	}
	delete[] houses;
}
*/