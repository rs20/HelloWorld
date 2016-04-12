
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//#include "Algorithm.cpp"
//#include "313178576_A_.cpp"
#include "313178576_B_.cpp"
//#include "313178576_C_.cpp"


#define MORE_THAN_6_ARGUMENTS "Expecting maximum of 6 arguments, instead got "
#define WRONG_ARGUMENTS_NUM "Incorrect number of arguments"
#define WRONG_ARGUMENTS "Incorrect use of flag; only '-config', '-house_path' and 'algorithm_path' flags are acceptable"
#define BATTERY_DEAD "Battery's Dead! Game Over."
#define NO_MORE_MOVES "Time's up! No more moves."
#define INTO_WALL "Robot walked into a wall! Game Over."

#ifndef __MIN_
#define __MIN
#define MIN(a,b) (((a)<(b)) ? (a):(b))
#endif

#define MAX_SCORE 2050