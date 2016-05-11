using namespace std;
#include <string>
#include <map>

/*
the implementation of calc_score shall be (optional) dynamically loaded with 'score_formula.so'
map contains:
1. actual position in competition
2. simulation steps
3. winner num steps
4. this num steps
5. sum dirt in house
6. dirt collected
7. is back in docking
*/
extern "C" {
	int calc_score(const map<string, int>& score_params);
}