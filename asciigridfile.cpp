#include "asciigridfile.h"

int ASCIIGridFile::load(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open the asciifile.");
	}

	std::string buff;
	std::getline(file, buff);// first line
	ncols = getIntValue(buff);
	if (ncols <= 0) {
		throw std::runtime_error("ASCIIfile ncols less than 0");
	}

	std::getline(file, buff);// second line
	nrows = getIntValue(buff);
	if (nrows <= 0) {
		throw std::runtime_error("ASCIIfile nrows less than 0");
	}

	std::getline(file, buff);// thrid line
	LEFT_X = getDoubleValue(buff);

	std::getline(file, buff);// forth line
	DOWN_Y = getDoubleValue(buff);

	std::getline(file, buff);// fifth line
	PixelSizeX = PixelSizeY = getDoubleValue(buff);

	RIGHT_X = LEFT_X + PixelSizeX * ncols;
	UP_Y = DOWN_Y + PixelSizeY * nrows;

	std::getline(file, buff);// sixth line跳过这行
	map.reAlloc(ncols, nrows);
	for (int i = 0; i < nrows; i++) {
		if (!std::getline(file, buff)) {// 读取dem数据
			throw std::runtime_error("Failed to read line" + std::to_string(i + 1));
		}
		std::istringstream iss(buff);
		for (int j = 0; j < ncols; j++) {
			float value;
			if (!(iss >> value)) {
				throw std::runtime_error("Failed to read float value at row"+ 
					std::to_string(i + 1) + ", column " + std::to_string(j + 1));
			}
			map[j][i] = value;
		}
	}

	file.close();

	maxVal = minVal = map[0][0];
	map.genMaxMin(&minVal, &maxVal);

	return 0;
}

int ASCIIGridFile::getIntValue(const std::string& line) {
	return std::stoi(line.substr(line.find_last_of(' ') + 1));
}
double ASCIIGridFile::getDoubleValue(const std::string& line) {
	return std::stod(line.substr(line.find_last_of(' ') + 1));
}