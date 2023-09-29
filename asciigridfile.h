#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "array2d.h"

class ASCIIGridFile
{
private:
	int getIntValue(const std::string& line);
	double getDoubleValue(const std::string& line);

public:
	double RIGHT_X, LEFT_X, UP_Y, DOWN_Y;
	double PixelSizeX, PixelSizeY;
	int ncols, nrows;
	Array2D<float> map;
	float minVal, maxVal;

	int load(const std::string & path);
	
};

