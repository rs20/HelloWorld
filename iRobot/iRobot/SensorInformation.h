#ifndef __SENSOR_INFORMATION__H_
#define __SENSOR_INFORMATION__H_

struct SensorInformation
{
	int dirtLevel;
	bool isWall[4]; // [0] East, [1] West, [2] South, [3] North
};

#endif //__SENSOR_INFORMATION__H_