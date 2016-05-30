#include "Encoder.h"

#include <stdlib.h>
#include <string>

int Encoder::encode(const string& imagesString, const string& videoOutput)
{
	string ffmpegCmd = "ffmpeg -y -i " + imagesString + " " + videoOutput;
	int ret = system(ffmpegCmd.c_str());
	if (ret == -1)
	{
		return -1;
	}
	return 0;
}
