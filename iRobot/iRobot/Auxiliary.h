#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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
#endif


#define defaultConfigPath "config/"
#define defaultHousePath "house/"
#define defaultConfigFile "config.ini"

#define ERROR_HOUSE_FILE "Error opening the house file."
#define ERROR_CONFIG_FILE "Error opening the config file."
#define ERROR_OVERRIDE_DOCKING_STATION "Docking station will be overridden as it's placed under the surrounding walls."


void handleConfigFile(std::string configPath, std::map<std::string, int>& house);
std::wstring stringToWstring(const std::string& s);
void handleHouseFiles(std::string housePath, int numOfHouses, House* houses);
int getNumberOfHouses(std::string housePath);
void printHouseWithRobot(House& house);

std::vector<std::string> split(const std::string &s, char delimiter);
std::string trim(std::string& str);
