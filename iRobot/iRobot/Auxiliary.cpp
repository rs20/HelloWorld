#include "stdafx.h"
#include "Auxiliary.h"




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

int handleConfigFile(std::string configPath, std::map<std::string, int> &config)
{
	std::ifstream myfile(configPath + defaultConfigFile);
	std::string line;

	// update according to split 'sarel - המתרגל' did on last recitation
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			std::vector<std::string> tokens = split(line, '=');
			if (tokens.size() != 2)
				continue;
			config[trim(tokens[0])] = stoi(tokens[1]);
		}
		myfile.close();
	}
	else 
	{
		std::cout << ERROR_CONFIG_FILE << std::endl;
		return -1;
	}
	return 0;
}

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

void handleHouseFiles(std::string housePath, int numOfHouses, House* houses)
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
	if (pDIR = opendir(housePath))
	{
		while (entry = readdir(pDIR))
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
		return -1;
	}
#endif

	for (int k = 0; k < numOfHouses; k++)
	{
		bool goodHouse = true;
		std::ifstream myfile(housePath + fileNames[k]);
		std::string line;

		if (myfile.is_open()) {
			getline(myfile, line);
			houses[k].houseName = line;
			getline(myfile, line);
			houses[k].houseDescription = line;
			getline(myfile, line);
			houses[k].rows = stoi(line);
			getline(myfile, line);
			houses[k].cols = stoi(line);

			houses[k].initialSumOfDirt = 0;

			houses[k].matrix = new char*[houses[k].rows];
			for (int i = 0; i < houses[k].rows; i++) {
				houses[k].matrix[i] = new char[houses[k].cols];
				for (int j = 0; j < houses[k].cols; j++)
					houses[k].matrix[i][j] = ' ';
			}

			// [amir said at the forum that they will test with houses with rows and columns that match to the matrix
			// i.e. -> if row and colum says 8 and 10 repsectively, then the matrix will be 8x10
			// however -> we may need to fill in walls]

			// let's try to take care of more complicated houses ... (for targils 2,3..)

			/*
			int i = 0;
			while (getline(myfile, line) && i < houses[k].rows)
			{
				for (int j = 0; j < line.length(); j++)
				{
					houses[k].matrix[i][j] = line[j];
					// found robot's starting point == docking station
					if (line[j] == 'D') {
						houses[k].robotRow = i;
						houses[k].robotCol = j;
						houses[k].dockingRow = i;
						houses[k].dockingCol = j;
					}
					if (line[j] >= '1' && line[j] <= '9')
						houses[k].initialSumOfDirt += (line[j] - '0');
				}
				i++;
			}
			*/
			/*
			// check that the house is formatted as desired (rows and columns as written and walls set properly)
			if (i == houses[k].rows) { // rows match
				// check that the first row contains walls only or else - override with walls
				for (int j = 0; j < houses[k].cols; j++) {
					if (houses[k].matrix[0][j] != 'W') {
						if (houses[k].matrix[0][j] == 'D') {
							std::cout << ERROR_OVERRIDE_DOCKING_STATION << std::endl;
							houses[k].isValidHouse = false;
							goodHouse = false;
							break;
						}
						houses[k].matrix[0][j] = 'W';
					}
				}
				if (!goodHouse)
					continue; // to another house
				// check that the last row contains walls only or else - override with walls
				for (int j = 0; j < houses[k].cols; j++) {
					if (houses[k].matrix[houses[k].rows-1][j] != 'W') {
						if (houses[k].matrix[houses[k].rows][j] == 'D') {
							std::cout << ERROR_OVERRIDE_DOCKING_STATION << std::endl;
							houses[k].isValidHouse = false;
							goodHouse = false;
							break;
						}
						houses[k].matrix[houses[k].rows][j] = 'W';
					}
				}
				if (!goodHouse)
					continue; // to another house
				// check that the most left column contains only walls or else - override with walls
				for (int j = 0; j < houses[k].rows; j++) {
					if (houses[k].matrix[j][0] != 'W') {
						if (houses[k].matrix[j][0] == 'D') {
							std::cout << ERROR_OVERRIDE_DOCKING_STATION << std::endl;
							houses[k].isValidHouse = false;
							goodHouse = false;
							break;
						}
						houses[k].matrix[j][0] = 'W';
					}
				}
				if (!goodHouse)
					continue; // to another house
				// check that the most right column contains walls only or else - override with walls
				for (int j = 0; j < houses[k].rows; j++) {
					if (houses[k].matrix[j][houses[k].cols-1] != 'W') {
						if (houses[k].matrix[j][houses[k].cols - 1] == 'D') {
							std::cout << ERROR_OVERRIDE_DOCKING_STATION << std::endl;
							houses[k].isValidHouse = false;
							goodHouse = false;
							break;
						}
						houses[k].matrix[j][houses[k].cols-1] = 'W';
					}
				}
				if (!goodHouse)
					continue; // to another house
				// Done! the house is formatted as desired
			}
			else {

			}
			*/
			
			for (int i = 0; i < houses[k].rows; i++)
			{
				getline(myfile, line);
				for (int j = 0; j < houses[k].cols; j++)
				{
					houses[k].matrix[i][j] = line[j];
					// found robot's starting point == docking station
					if (line[j] == 'D') {
						houses[k].robotRow = i;
						houses[k].robotCol = j;
						houses[k].dockingRow = i;
						houses[k].dockingCol = j;
					}
					if (line[j] >= '1' && line[j] <= '9')
						houses[k].initialSumOfDirt += (line[j] - '0');
				}
			}

			//filling the house walls
			for (int i = 0; i < houses[k].rows; i++)
			{
				houses[k].matrix[i][houses[k].cols -1] = 'W';
			}

			for (int j = 0; j < houses[k].cols; j++)
			{
				houses[k].matrix[houses[k].rows-1][j] = 'W';
			}
			

			houses[k].sumOfDirt = houses[k].initialSumOfDirt;
			houses[k].isValidHouse = true;
			myfile.close();
		}
		else {
			std::cout << ERROR_HOUSE_FILE << std::endl;
			houses[k].isValidHouse = false;
		}
	}
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
		std::cout << ERROR_HOUSE_PATH << std::endl;
		return -1;
	}

#else
	DIR *pDIR;
	struct dirent *entry;
	std::string temp = "";
	if (pDIR = opendir(housePath))
	{
		while (entry = readdir(pDIR))
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
		std::cout << ERROR_HOUSE_PATH << std::endl;
		return -1;
	}
#endif
	return numOfHouses;
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