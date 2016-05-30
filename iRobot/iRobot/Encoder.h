#ifndef _MONTAGE__H_
#define _MONTAGE__H_

#include <vector>
#include <string>
using namespace std;

class Encoder
{
public:
	static int encode(const string& imagesString, const string& videoOutput);
};

#endif //_MONTAGE__H_
