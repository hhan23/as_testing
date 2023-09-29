#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "function.h"

class SimplePolyline
{
public:
	std::vector<double> x;
	std::vector<double> y;
};

class ShapeFilePolyline
{
public:
	int fileLen;
	std::vector<SimplePolyline> plls;
	double Xmin, Ymin, Xmax, Ymax;

	int loadSHP(const std::string& path);
};

