
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>


// include for sleep
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifndef __HOUSE_H
#define __HOUSE_H
#include "House.h"
#endif

#include "Algorithm.cpp"



#define MORE_THAN_4_ARGUMENTS "Expecting maximum of 4 arguments, instead got "
#define WRONG_ARGUMENTS_NUM "incorrect number of arguments"
#define BATTERY_DEAD "Battery's Dead! Game Over."
#define ERROR_HOUSE_FILE "Error opening the house file."
#define ERROR_CONFIG_FILE "Error opening the config file."
#define NO_MORE_MOVES "Time's up! No more moves."

#define defaultConfigPath "config/"
#define defaultHousePath "house/"
#define defaultConfigFile "config.ini"

#define MAX(a,b) (((a)>(b)) ? (a):(b))
#define MIN(a,b) (((a)<(b)) ? (a):(b))


wstring stringToWstring(const std::string& s)
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