#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <queue>
#include <filesystem>

#include "array2d.h"
#include "globaldata.h"
#include "enums.h"

#define GRAVITY_CONSTANT 9.8

class Solver
{
private:
    void addEdge(int cellID, int edgeID, bool isNeg);
    void divideGrid();
    void calcDrains();
    void calcBuildingRain();
public:
    int w, h;
    double currT;
    double deltaT;
    double rainT;
    int reportT;
    int totalT;
    double rainfall;
    double deltaX;
    int edgeNum;
    Edge* edges;
    int cellNum;
    Cell* cells;
    int** IDs;

    Solver();
    ~Solver();
    int init();
    int calcStep();
    int outputDepthData(const std::string& path);
    int run(const std::string& dirPath);
};