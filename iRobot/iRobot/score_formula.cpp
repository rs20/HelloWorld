#include "score_formula.h"

#define MAX(a,b) (((a)>(b)) ? (a):(b))

extern "C" {
	int calc_score(const map<string, int>& score_params)
	{
		int position_in_competition = score_params.at("actual_position_in_competition");
		//int simulation_steps = score_params.at("simulation_steps");
		int winner_num_steps = score_params.at("winner_num_steps");
		int this_num_steps = score_params.at("this_num_steps");
		int sum_dirt_in_house = score_params.at("sum_dirt_in_house");
		int dirt_collected = score_params.at("dirt_collected");
		int is_back_in_docking = score_params.at("is_back_in_docking");

		if (winner_num_steps == 0 && this_num_steps == 0)
		{
			return 2050;
		}

		if (is_back_in_docking && sum_dirt_in_house == dirt_collected)
		{
			if (position_in_competition > 4)
			{
				position_in_competition = 4;
			}
		}
		else
		{
			position_in_competition = 10;
		}

		return MAX(0, 2000
			- 50 * (position_in_competition - 1)
			+ 10 * (winner_num_steps - this_num_steps)
			- 3 * (sum_dirt_in_house - dirt_collected)
			+ ((is_back_in_docking) ? 50 : -200)); // +50 if back in docking station, -200 if not
	}
}