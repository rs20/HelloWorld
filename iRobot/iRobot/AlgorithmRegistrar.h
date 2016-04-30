// AlgorithmRegistrar.h is part of the simulation project ONLY
#ifndef __ALGORITHM__REGISTRAR__H
#define __ALGORITHM__REGISTRAR__H

#include <functional>
#include <memory>
#include <list>

// includes of linux
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#include "Direction.h"
#include "AbstractAlgorithm.h"

// AlgorithmRegistrar is a singleton -> there's only one instance used by everyone
class AlgorithmRegistrar {
	// all 4 lists are corresponding; i.e. they're all of the same size
	// and index i in each of them represensts algorithm no. i
	std::list<std::string> algorithmNames;
	std::list<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
	std::list<void*> hndls;
	std::list<std::string> errorsList; // errors of algorithms in load time

	void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
		instance.algorithmFactories.push_back(algorithmFactory);
	}
	void setNameForLastAlgorithm(const std::string& algorithmName) {
		//assert(algorithmFactories.size() - 1 == algorithmNames.size());
		algorithmNames.push_back(algorithmName);
	}
public:
	// AlgorithmRegistration accesses the private method 'registerAlgorithm' of AlgorithmRegistrar
	friend class AlgorithmRegistration;

	// FILE_CANNOT_BE_LOADED = the file could not be found / the 'dlopen' failed / no permissions
	// NO_ALGORITHM_REGISTERED = opened but did not register (how can we tell? factories size did not increase)
	enum { ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };

	// the simulation uses this to load algorithms [implemented in the cpp file]
	int loadAlgorithm(const std::string& path, const std::string& so_file_name_without_so_suffix);

	list<unique_ptr<AbstractAlgorithm>> getAlgorithms()const;

	const std::list<std::string>& getAlgorithmNames()const {
		return algorithmNames;
	}
	const std::list<std::string>& getErrorsList()const {
		return errorsList;
	}
	const std::list<void*>& getHndlsList()const {
		return hndls;
	}
	size_t size()const {
		return algorithmFactories.size();
	}
	static AlgorithmRegistrar& getInstance() {
		return instance;
	}

	void clearFactories() {
		algorithmFactories.clear();
	}
	void clearHndls() {
		for (auto hndl : hndls)
			dlclose(hndl);
	}

	void addErrorToList(std::string err) {
		errorsList.push_back(err);
	}
	void addHndlToList(void* hndl) {
		hndls.push_back(hndl);
	}
private:
	// static member -> there's only one instance (no dynamic loading - no need to free)
	static AlgorithmRegistrar instance;
};

#endif //__ALGORITHM__REGISTRAR__H