.DEFAULT_GOAL := simulator

# place here your source files
src = Sensor.cpp Simulator.cpp Auxiliary.cpp AlgorithmRegistrar.cpp AlgorithmRegistration.cpp MyHouse.cpp SuperAlgorithm.cpp
obj = $(src:.cpp=.o)

CXXFLAGS = -std=c++11 -g -ldl -rdynamic -Wall -O2 -pedantic -lpthread

# may add boost libraries here
LD_FLAGS = -lm

simulator: $(obj)
	g++ -o $@ $^ $(CXXFLAGS)
	g++ -std=c++11 -shared -fPIC -o score_formula.so score_formula.cpp
	g++ -std=c++11 -shared -fPIC -o 313178576_A_.so 313178576_A_.cpp
	g++ -std=c++11 -shared -fPIC -o 313178576_B_.so 313178576_B_.cpp
	g++ -std=c++11 -shared -fPIC -o 313178576_C_.so 313178576_C_.cpp
.PHONY: clean
clean:
	rm -f $(obj) simulator
	rm -f score_formula.so 313178576_A_.so 313178576_B_.so 313178576_C_.so