#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "array2d.h"
#include "asciigridfile.h"
#include "shapefilepoint.h"
#include "shapefilepolygon.h"
#include "shapefilepolyline.h"


class GlobalData
{
public:
	static std::string path;
	static int w, h;
	static Array2D<char> isBuilding;
	static Array2D<double> z;
	static Array2D<double> d0;
	static double x1, x2, y1, y2;
	//time
	static double totalTime;
	static double rainTime;
	static double reportTime;
	static double deltaT;
	//function
	static bool building;
	static bool drain;
	static bool swmm;
	static bool rain;
	static bool infiltration;
	//File path
	static std::string DemFilePath;
	static std::string BuildingFilePath;
	static std::string DrainFilePath;
	static char* f1,* f2,* f3;
	//Physics
	static double manningN;
	static double epsilon;
	static double courant;	
	static double rainfallIntensity;
	static double deltaX;
	//Mode
	static int outputMode;
	static int buildinginputMode;
	
	
	static std::vector<Point<double>> drains;//≈≈ÀÆŒª÷√
	static std::vector<Drain> drainList;
	static double drainA, drainC;
	static double baseHeight;

	static void genXDamBreak(int tw, double dHigh, double dLow);
	static void genYDamBreak(int th, double dHigh, double dLow);
	static void genXDamBreakSlope(int tw, double dHigh, double slope);
	static void genSquareDamBreak(int ta, double dHigh, double dLow);
	static void genRandomBox(int tw, int th, double zHigh, double zLow, double elevW);

	static int initalinpfile();
	static void inital(ASCIIGridFile& gf);	
	static void freshBuildings(ASCIIGridFile& gf);
	static void freshBuildings(ShapeFilePolygon& sfpg);
	static void freshDrains(ShapeFilePoint& sfp);
	static int freshDrains(char* f1, char* f2, char* f3);
	static void deinit();
};