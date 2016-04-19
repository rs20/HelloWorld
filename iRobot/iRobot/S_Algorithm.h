
#ifndef __S_ALGORIGHTM_H
#define __S_ALGORIGHTM_H

#include <string>
#include "AbstractAlgorithm.h"

typedef struct s_Algorithm
{
	// information based on .so file
	std::string algorithmFileName;
	AbstractAlgorithm* algo;
	void *hndl = NULL;
	// information the program figured out
	bool isValidAlgorithm; // flag if the house is formatted properly or not
	std::string error;
} s_Algorithm;

#endif