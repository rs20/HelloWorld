#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <list>
#include <iomanip>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#ifdef __linux__
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#endif

#include "AlgorithmRegistrar.h"
#include "House.h"
// House.h includes 'make_unique'

#ifndef __MIN_
#define __MIN_
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

#define MAX_SCORE 2050

// OLD Paths
/*
#define defaultConfigPath "config/"
#define defaultHousePath "house/"
#define defaultAlgorithmPath "Algorithms/"
*/
#define defaultConfigPath ""
#define defaultHousePath ""
#define defaultAlgorithmPath ""
#define defaultConfigFile "config.ini"

#define USAGE "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>]"

#define ERROR_OPEN_HOUSE_FILE "cannot open file"
#define ERROR_CONFIG_FILE1 "config.ini exists in "
#define ERROR_CONFIG_FILE2 " but cannot be opened"
#define ERROR_OVERRIDE_DOCKING_STATION "Docking station will be overridden as it's placed under the surrounding walls."
#define ERROR_NO_DOCKING_STATIONS "missing docking station (no D in house)"
#define ERROR_TOO_MANY_DOCKING_STATIONS "too many docking stations (more than one D in house)"

#define NOT_VALID_SO "file cannot be loaded or is not a valid .so"
#define NOT_VALID_ALGORITHM "valid .so, but no algorithm was registered after loading it"

#define MORE_THAN_6_ARGUMENTS "Expecting maximum of 6 arguments, instead got "
#define WRONG_ARGUMENTS_NUM "Incorrect number of arguments"
#define WRONG_ARGUMENTS "Incorrect use of flag; only '-config', '-house_path' and 'algorithm_path' flags are acceptable"
#define BATTERY_DEAD "Battery's Dead! Game Over."
#define NO_MORE_MOVES "Time's up! No more moves."
#define INTO_WALL "Robot walked into a wall! Game Over."


std::string handleSlash(const char* path);
int handleConfigFile(std::string configPath, std::map<std::string, int>& config);
std::wstring stringToWstring(const std::string& s);
int handleHouseFiles(std::string housePath, int numOfHouses, House* houses);
int getNumberOfHouses(std::string housePath);
int getNumberOfPotentialAlgorithms(std::string algorithmPath);
int handleAlgorithmFiles(std::string algorithmPath, int numOfAlgorithms, AlgorithmRegistrar& algorithms);
void printHouseWithRobot(House& house);
void freeHouses(House *houses, int numOfHouses);
void copyHouse(House& dst, House& src);

std::vector<std::string> split(const std::string &s, char delimiter);
std::string trim(std::string& str);
