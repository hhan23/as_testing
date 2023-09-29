#include "swmminterface.h"
#include "swmm5.h"


int SWMMInterface::init(char* f1, char* f2, char* f3)
{
    //初始化swmm设置
	swmm_open(f1, f2, f3);
	swmm_initInterface(&drainCount, &drainName);
	depthCache = new double[drainCount];
	depthCacheOut = new double[drainCount];
	surcharge = new bool[drainCount];
	swmm_start(1);
	return 0;
}

void SWMMInterface::deinit()
{
    //释放内存，中止swmm
    drainCount = 0;
    drainName = 0;
    swmm_end();
    delete[] depthCache;
    depthCache = 0;
    delete[] depthCacheOut;
    depthCacheOut = 0;
    delete[] surcharge;
    surcharge = 0;
    swmm_deinitInterface();
    swmm_close();
}

int SWMMInterface::findDrain(const std::string& path)
{
    for (int i = 0; i < drainCount; i++) {
        if (path == drainName[i]) {
            return i;
        }
    }
    return -1;
}

void SWMMInterface::setDepths()
{
    swmm_setDepths(depthCache);
}

void SWMMInterface::getDepths()
{
    swmm_getDepths(depthCacheOut);
}

int SWMMInterface::routingStep(double deltaT)
{
    return swmm_step(&deltaT);
}
