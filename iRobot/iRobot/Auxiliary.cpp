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

void handleConfigFile(std::string configPath, std::map<std::string, int> &config)
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
	else {
		std::cout << ERROR_CONFIG_FILE << std::endl;
	}
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

void handleHouseFile(std::string housePath, House& house)
{
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
		} while (FindNextFile(hFile, &fd));
		FindClose(hFile);
	}

#endif
	//need to add an option for linux using readdir and #include <dirent.h>

	std::ifstream myfile(housePath + fileName);
	std::string line;


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

		// check that the outer rows and columns contain walls only
		for (int i = 0; i < house.rows; i++)
		{
			// TODO: handle houses with non walls only on outer rows/columns
			// TODO: handle non good formatted houses
		}
	}
	else {
		std::cout << ERROR_HOUSE_FILE << std::endl;
	}
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