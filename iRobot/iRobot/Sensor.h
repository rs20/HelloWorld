
#include "AbstractSensor.h"
#include "House.h"

class Sensor : public AbstractSensor
{
private:
	House* house;
public:
	// pass the house to the sensor (by reference)
	Sensor(House *h);
	virtual SensorInformation sense() const override;
};