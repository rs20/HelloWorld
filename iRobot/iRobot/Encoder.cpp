#include "Encoder.h"

#include <stdlib.h>
#include <string>

void Encoder::encode(const string& imagesString, const string& videoOutput)
{
  string ffmpegCmd = "ffmpeg -y -i " + imagesString + " " + videoOutput;
  int ret = system(ffmpegCmd.c_str());
  if (ret == -1)
  {
    //handle error
  }
}

