#pragma once
#include <string>

class SWMMInterface
{
public:
    static int drainCount;
    static char** drainName;
    static double* depthCache;
    static double* depthCacheOut;
    static bool* surcharge;

    static int init(char* f1, char* f2, char* f3);
    static void deinit();
    static int findDrain(const std::string& path);
    static void setDepths();
    static void getDepths();
    static int routingStep(double deltaT);
};