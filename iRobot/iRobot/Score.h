#ifndef __SCORE__H
#define __SCORE__H

#define MAX(a,b) (((a)>(b)) ? (a):(b))

int score(int position_in_competition, int winner_num_steps, int this_num_steps, int dirt_collected, int sum_dirt_in_house, bool is_back_in_docking)
{
	return MAX(0, 2000
		- 50 * (position_in_competition - 1)
		+ 10 * (winner_num_steps - this_num_steps)
		- 3 * (sum_dirt_in_house - dirt_collected)
		+ ((is_back_in_docking) ? 50 : -200)); // +50 if back in docking station, -200 if not
}

#endif // __SCORE__H