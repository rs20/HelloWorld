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

#include <thread>
#include <atomic>
//#include <mutex>
// we did not use a mutex at all, only atomic integers. the threads work on the same vectors/arrays ...
// but at each time, access a different index (the index of the house they currently work on) in the vector/array

// linux includes
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "AlgorithmRegistrar.h"
#include "House.h"
// House.h includes 'make_unique'

#ifndef __MIN_
#define __MIN_
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

#define MAX_SCORE 2050

#define defaultConfigPath ""
#define defaultHousePath ""
#define defaultAlgorithmPath ""
#define defaultScorePath ""
#define defaultConfigFile "config.ini"
#define defaultScoreFile "score_formula.so"
#define defaultThreads "1"

#define USAGE "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>] \
[-score_formula <score .so path>] [-threads <num threads>]"

#define ERROR_CALCULATE_SCORE "Score formula could not calculate some scores, see -1 in the results table"
#define ERROR_FIND_SCORE_FILE "cannot find score_formula.so in "
#define ERROR_FIND_ALGORITHM_FILES "cannot find algorithm files in "
#define ERROR_FIND_HOUSE_FILES "cannot find house files in "
#define ERROR_NOT_VALID_SCORE "score_formula.so is valid .so but it does not have a valid score formula"
#define ERROR_FIND_CONFIG_FILE "cannot find config.ini in "
#define ERROR_CONFIG_FILE_BAD_VALUE "config.ini having bad value for "
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

std::vector<std::string> split(const std::string &s, char delimiter);
std::string trim(std::string& str);
std::string handleSlash(const char* path);
std::wstring stringToWstring(const std::string& s);

int handleConfigFile(std::string configPath, std::map<std::string, int>& config);
int handleScoreFile(std::string scorePath, void* hndl, int(**score_function)(const map<string, int>& score_params));
int getNumberOfHouses(std::string housePath, vector<string>& houseFileNames);
int readHouseFile(int houseIndex, string houseFileName, string* houseErrors, House* house);
int getNumberOfPotentialAlgorithms(std::string algorithmPath);
int handleAlgorithmFiles(std::string algorithmPath, int numOfAlgorithms, AlgorithmRegistrar& algorithms);

void printHouseWithRobot(House& house);
void copyHouse(House& dst, House* house);
std::string fullPath(std::string relativePath);