// AlgorithmRegistration.cpp is part of the simulation project ONLY

#include "AlgorithmRegistrar.h"
#include "AlgorithmRegistration.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}