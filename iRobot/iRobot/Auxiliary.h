#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#ifndef __HOUSE_H
#define __HOUSE_H
#include "House.h"
#endif

// include for sleep
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif


#define defaultConfigPath "config/"
#define defaultHousePath "house/"
#define defaultAlgorithmPath "algorithm/"
#define defaultConfigFile "config.ini"

#define ERROR_OPEN_HOUSE_FILE "cannot open file"
#define ERROR_CONFIG_FILE1 "config.ini exists in "
#define ERROR_CONFIG_FILE2 " but cannot be opened"
#define ERROR_OVERRIDE_DOCKING_STATION "Docking station will be overridden as it's placed under the surrounding walls."
#define ERROR_NO_DOCKING_STATIONS "missing docking station (no D in house)"
#define ERROR_TOO_MANY_DOCKING_STATIONS "too many docking stations (more than one D in house)"

std::string handleSlash(const char* path);
int handleConfigFile(std::string configPath, std::map<std::string, int>& house);
std::wstring stringToWstring(const std::string& s);
int handleHouseFiles(std::string housePath, int numOfHouses, House* houses);
int getNumberOfHouses(std::string housePath);
int getNumberOfAlgorithms(std::string algorithmPath);
int handleAlgorithmFiles(std::string algorithmPath, int numOfAlgorithm); // TODO: change it someday soon ...
void printHouseWithRobot(House& house);
void usageMessage(std::string configPath, std::string housePath, std::string algorithmPath);

void copyHouse(House& dst, House& src);

std::vector<std::string> split(const std::string &s, char delimiter);
std::string trim(std::string& str);
