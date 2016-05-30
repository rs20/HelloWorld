#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>

#include "House.h"
#include "Montage.h"


void createDirectoryIfNotExists(const string& dirPath)
{
	std::string cmd = "mkdir -p " + dirPath;
	int ret = system(cmd.c_str());
	if (ret == -1)
	{
		//handle error
	}
}

void House::montage(const std::string& algoName)
{
	std::vector<std::string> tiles;
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			if (row == robot.row && col == robot.col)
				tiles.push_back("R");
			else if (matrix[row][col] == ' ')
				tiles.push_back("0");
			else
				tiles.push_back(std::string() + matrix[row][col]);
		}
	}

	std::string imagesDirPath = "simulations/" + algoName + "_" + houseFileName;
	createDirectoryIfNotExists(imagesDirPath);
	std::string counterStr = std::to_string(picCounter++);
	std::string composedImage = imagesDirPath + "/image" + std::string(5 - counterStr.length(), '0') + counterStr + ".jpg";
	if (Montage::compose(tiles, cols, rows, composedImage) == false)
	{
		videoError = true;
	}
}