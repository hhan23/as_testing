#include "input.h"
using inp = input;

bool inp::ifignored(std::string& line)
{
	return (line.empty() || line[0] == '#');
}

void inp::readTimeModule(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		if (ifignored(line)) {
			continue;
		}
		else {
			// 移除前导和尾随空白
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
				return !std::isspace(ch);
				}));
			line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), line.end());
			if (!line.empty() && line[0] == '[') {
				// 读到新模块，结束此次读取
				break;
			}
			std::istringstream iss(line);
			std::string key;
			double value;
			if (!(iss >> key >> value)) {
				throw std::runtime_error("Error Format in Inputfile");
			}
			if (key == "totalT") {
				GlobalData::totalTime = value;
			}
			else if (key == "reportT") {
				GlobalData::reportTime = value;
			}
			else if (key == "rainT") {
				GlobalData::rainTime = value;
			}
			else if (key == "deltaT") {
				GlobalData::deltaT = value;
			}
			//若更多参数
		}
	}
}

void inp::readFunctionModule(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		if (ifignored(line)) {
			continue;
		}
		else {
			// 移除前导和尾随空白
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
				return !std::isspace(ch);
				}));
			line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), line.end());
			if (!line.empty() && line[0] == '[') {
				// 读到新模块，结束此次读取
				break;
			}
			std::istringstream iss(line);
			std::string key;
			bool value;
			if (!(iss >> key >> value)) {
				throw std::runtime_error("Error Format in Inputfile");
			}
			if (key == "Building") {
				GlobalData::building = value;
			}
			else if (key == "Drain") {
				GlobalData::drain = value;
			}
			else if (key == "Rain") {
				GlobalData::rain = value;
			}
			else if (key == "Infiltration") {
				GlobalData::infiltration = value;
			}
			else if (key == "SWMM") {
				GlobalData::swmm = value;
			}
			//若更多参数
		}
	}
}

void inp::readFileModule(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		if (ifignored(line)) {
			continue;
		}
		else {
			// 移除前导和尾随空白
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
				return !std::isspace(ch);
				}));
			line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), line.end());
			if (!line.empty() && line[0] == '[') {
				// 读到新模块，结束此次读取
				break;
			}
			std::istringstream iss(line);
			std::string key;
			std::string value;
			if (!(iss >> key >> value)) {
				throw std::runtime_error("Error Format in Inputfile");
			}
			if (key == "DemFile") {
				GlobalData::DemFilePath = value;
			}
			else if (key == "BuildingFile") {
				GlobalData::BuildingFilePath = value;
			}
			else if (key == "DrainFile") {
				GlobalData::DrainFilePath = value;
			}
			//若更多参数
		}
	}
}

void inp::readPhysicsModule(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		if (ifignored(line)) {
			continue;
		}
		else {
			// 移除前导和尾随空白
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
				return !std::isspace(ch);
				}));
			line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), line.end());
			if (!line.empty() && line[0] == '[') {
				// 读到新模块，结束此次读取
				break;
			}
			std::istringstream iss(line);
			std::string key;
			double value;
			if (!(iss >> key >> value)) {
				throw std::runtime_error("Error Format in Inputfile");
			}
			if (key == "courant") {
				GlobalData::courant = value;
			}
			else if (key == "manningN") {
				GlobalData::manningN = value;
			}
			else if (key == "epsilon") {
				GlobalData::epsilon = value;
			}
			else if (key == "rainfallIntensity") {
				GlobalData::rainfallIntensity = value;
			}
			//若更多参数
		}
	}
}

void inp::readModeModule(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		if (ifignored(line)) {
			continue;
		}
		else {
			// 移除前导和尾随空白
			line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) {
				return !std::isspace(ch);
				}));
			line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), line.end());
			if (!line.empty() && line[0] == '[') {
				// 读到新模块，结束此次读取
				break;
			}
			std::istringstream iss(line);
			std::string key;
			int value;
			if (!(iss >> key >> value)) {
				throw std::runtime_error("Error Format in Inputfile");
			}
			if (key == "outputMode") {
				GlobalData::outputMode = value;
			}
			else if (key == "buildinginputMode") {
				GlobalData::buildinginputMode = value;
			}
			//若更多参数
		}
	}
}

int inp::input_readData(std::ifstream& file)
{
	std::string line;
	while (std::getline(file, line)) {
		std::getline(file, line);
		std::string key;
		std::istringstream iss(line);

		iss >> key;
		if (ifignored(line)) {
			continue;
		}
		else {
			if (key == "[Time]") {
				readTimeModule(file);
			}
			else if (key == "[Function]") {
				readFunctionModule(file);
			}
			else if (key == "[File]") {
				readFileModule(file);
			}
			else if (key == "[Physics]") {
				readPhysicsModule(file);
			}
			else if (key == "[Mode]") {
				readModeModule(file);
			}
		}
	}
	return 0;
}