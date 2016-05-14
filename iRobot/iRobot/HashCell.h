#ifndef __HASH_H_
#define __HASH_H

#include "Cell.h"

// hash function on cells:
// on cell (a,b) return ax+b when x=
struct HashCell {
	// choose x to be the maximum of b
	int x = 1000;
	int operator()(const Cell& cell) const
	{
		return (cell.row * x + cell.col);
	}
};

#endif // __HASH_H



// CURRENTLY NOT USED BECAUSE DOESNT RUN FASTER THAN THE REGULAR MAP<CELL,CHAR>