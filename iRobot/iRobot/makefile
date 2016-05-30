CC = g++
LIBS = 

.cc.o:
	$(CC) -ggdb -c $<

default:
	make all

OBJS = simulator.o

simulator: Sensor.cpp Simulator.cpp Auxiliary.cpp AlgorithmRegistrar.cpp AlgorithmRegistration.cpp MyHouse.cpp SuperAlgorithm.cpp
	g++ -std=c++11 -rdynamic -o $@ $^ -std=c++11 -Wall -Wextra -O2 -pedantic -pthread -ldl

Sensor.o: Sensor.cpp Sensor.h AbstractSensor.h SensorInformation.h House.h Cell.h MakeUnique.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread Sensor.cpp

Simulator.o: Simulator.cpp Simulator.h Score.h Auxiliary.h Sensor.h House.h Cell.h MakeUnique.h AbstractSensor.h SensorInformation.h AlgorithmRegistrar.h AbstractAlgorithm.h Direction.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread Simulator.cpp

Auxiliary.o: Auxiliary.cpp Auxiliary.h House.h Cell.h MakeUnique.h AbstractSensor.h SensorInformation.h AlgorithmRegistrar.h AbstractAlgorithm.h Direction.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread Auxiliary.cpp

AlgorithmRegistrar.o: AlgorithmRegistrar.cpp AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread AlgorithmRegistrar.cpp

AlgorithmRegistration.o: AlgorithmRegistration.cpp AlgorithmRegistration.h AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread AlgorithmRegistration.cpp

MyHouse.o: MyHouse.cpp MyHouse.h Direction.h SensorInformation.h Cell.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread MyHouse.cpp

SuperAlgorithm.o: SuperAlgorithm.cpp SuperAlgorithm.h MakeUnique.h AlgorithmRegistration.h AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h MyHouse.h Cell.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic -pthread SuperAlgorithm.cpp

score_formula.o: score_formula.cpp
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic score_formula.cpp -mcmodel=large

score_formula.so: score_formula.o
	g++ -std=c++11 -shared -Wl,-soname,score_formula.so -o score_formula.so score_formula.o

313178576_A_.o: 313178576_A_.cpp 313178576_A_.h SuperAlgorithm.h MakeUnique.h AlgorithmRegistration.h AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h MyHouse.h Cell.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic 313178576_A_.cpp -mcmodel=large

313178576_A_.so: 313178576_A_.o
	g++ -std=c++11 -shared -Wl,-soname,313178576_B_.so -o 313178576_A_.so 313178576_A_.o

313178576_B_.o: 313178576_B_.cpp 313178576_B_.h SuperAlgorithm.h MakeUnique.h AlgorithmRegistration.h AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h MyHouse.h Cell.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic 313178576_B_.cpp -mcmodel=large

313178576_B_.so: 313178576_B_.o
	g++ -std=c++11 -shared -Wl,-soname,313178352_B_.so -o 313178576_B_.so 313178576_B_.o

313178576_C_.o: 313178576_C_.cpp 313178576_C_.h SuperAlgorithm.h MakeUnique.h AlgorithmRegistration.h AlgorithmRegistrar.h Direction.h AbstractAlgorithm.h AbstractSensor.h SensorInformation.h MyHouse.h Cell.h
	g++ -std=c++11 -c -std=c++11 -Wall -Wextra -pedantic 313178576_C_.cpp -mcmodel=large

313178576_C_.so: 313178576_C_.o
	g++ -std=c++11 -shared -Wl,-soname,313178576_C_.so -o 313178576_C_.so 313178576_C_.o

all: score_formula.o score_formula.so 313178576_A_.o 313178576_A_.so 313178576_B_.o 313178576_B_.so 313178576_C_.o 313178576_C_.so SuperAlgorithm.o MyHouse.o Sensor.o Auxiliary.o AlgorithmRegistrar.o AlgorithmRegistration.o Simulator.o simulator

clean:
	rm -f *.so *.o simulator