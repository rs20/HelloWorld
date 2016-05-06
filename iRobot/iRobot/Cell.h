#ifndef __CELL__H_
#define __CELL__H_

struct Cell
{
	int row;
	int col;

	bool operator<(const Cell &other) const
	{
		return ((this->row < other.row) || ((this->row == other.row) && (this->col < other.col)));
	};
	bool operator!=(const Cell &other) const
	{
		return ((this->row != other.row) || (this->col != other.col));
	}
	bool operator==(const Cell &other) const
	{
		return ((this->row == other.row) && (this->col == other.col));
	}
};

#endif //__CELL__H_