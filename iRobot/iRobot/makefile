.DEFAULT_GOAL := simulator

# place here your source files
src = Sensor.cpp Simulator.cpp Auxiliary.cpp AlgorithmRegistrar.cpp AlgorithmRegistration.cpp MyHouse.cpp
obj = $(src:.cpp=.o)

CXXFLAGS = -std=c++11 -g -ldl -rdynamic -Wall -O2 -pedantic

# may add boost libraries here
LD_FLAGS = -lm

simulator: $(obj)
	g++ -o $@ $^ $(CXXFLAGS)
	g++ -std=c++11 -shared -fPIC -o Smart1.so Smart1.cpp
.PHONY: clean
clean:
	rm -f $(obj) simulator
