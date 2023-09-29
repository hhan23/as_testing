#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>




class SimplePoint
{
public:
	double x, y;
};

class ShapeFilePoint
{
public:
	int fileLen;
	std::vector<SimplePoint> points;
	double Xmin, Ymin, Xmax, Ymax;

	int loadSHP(const std::string& path);
};