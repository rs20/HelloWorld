
#include "AlgorithmRegistrar.h"

AlgorithmRegistrar AlgorithmRegistrar::instance;

// sets the instances of all of the algorithms
list<unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms()const {
	list<unique_ptr<AbstractAlgorithm>> algorithms;
	for (auto algorithmFactoryFunc : algorithmFactories) {
		// tell algorithm to register itself and add to the algorithms list
		algorithms.push_back(algorithmFactoryFunc());
	}
	return algorithms;
}

// so_file_name is with suffix '.so'
int AlgorithmRegistrar::loadAlgorithm(const std::string& path, const std::string& so_file_name) {
	size_t size = instance.size();
	string tempAlgoPath = path + so_file_name;
	void* hndl = nullptr;
#ifdef _WIN32
	std::wstring stemp = std::wstring(tempAlgoPath.begin(), tempAlgoPath.end());
	LPCWSTR sw = stemp.c_str();
	hndl = LoadLibrary(sw);
#else
	hndl = dlopen(tempAlgoPath.c_str(), RTLD_NOW);
#endif
	if (hndl == NULL) {
		return FILE_CANNOT_BE_LOADED;
	}
	instance.addHndlToList(hndl);
	if (instance.size() == size) {
		return NO_ALGORITHM_REGISTERED; // no algorithm registered
	}
	instance.setNameForLastAlgorithm(so_file_name);
	return ALGORITHM_REGISTERED_SUCCESSFULY;
}