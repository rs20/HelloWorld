
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "Algorithm.cpp"


#define MORE_THAN_4_ARGUMENTS "Expecting maximum of 4 arguments, instead got "
#define WRONG_ARGUMENTS_NUM "incorrect number of arguments"
#define BATTERY_DEAD "Battery's Dead! Game Over."
#define NO_MORE_MOVES "Time's up! No more moves."
#define INTO_WALL "Robot walked into a wall! Game Over."

#define MIN(a,b) (((a)<(b)) ? (a):(b))