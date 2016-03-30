.DEFAULT_GOAL := simulator

# place here your source files
src = Sensor.cpp Simulator.cpp Score.cpp Auxiliary.cpp Algorithm.cpp
obj = $(src:.cpp=.o)

CXXFLAGS = -std=c++11 -g -Wall -O2 -pedantic -pthread

# may add boost libraries here
LD_FLAGS = -lm

simulator: $(obj)
	g++ -o $@ $^ $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) simulator
