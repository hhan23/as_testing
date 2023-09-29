// ‰»Îƒ£∞Â
/*
* -------------------------------------------------------------
* [Time]
* totalT
* reportT
* rainT
* deltaT
*
* [Function]
* Building
* Drain
* Rain
* Infiltration
*
* [File]
* Building
* Drain
* Rain
* 
* [Physics]
* courant
* manningN
* epsilon
* rainfallIntensity
*
* [Mode]
* Outputmode
*/
#pragma once
#include <fstream>
#include <iostream>
#include <istream>

#include "asciigridfile.h"
#include "enums.h"
#include "globaldata.h"

class input
{
private:
	static bool ifignored(std::string& line);
	static void readTimeModule(std::ifstream& file);
	static void readFunctionModule(std::ifstream& file);
	static void readFileModule(std::ifstream& file);
	static void readPhysicsModule(std::ifstream& file);
	static void readModeModule(std::ifstream& file);

public:
	static int input_readData(std::ifstream& file);
 };