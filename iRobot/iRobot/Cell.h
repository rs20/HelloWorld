#ifndef __CELL__H_
#define __CELL__H_

typedef struct Cell
{
	int row;
	int col;

	bool operator<(const Cell &other) const
	{
		return ((this->row < other.row) && (this->col < other.col));
	};
} Cell;

#endif //__CELL__H_