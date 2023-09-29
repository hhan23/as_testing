#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "function.h"

class SimplePolygon
{
public:
	std::vector<double> x;
	std::vector<double> y;
};

class ShapeFilePolygon
{
public:
	int fileLen;
	std::vector<SimplePolygon> plgs;
	double Xmin, Ymin, Xmax, Ymax;

	int loadSHP(const std::string& path);
	cv::Mat generateImage(double div, double l, double r, double b, double t);

	
};

int ShapeFilePolygon::loadSHP(const std::string& path)
{
	plgs.clear();
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open the shp file." << std::endl;
		return 1;
	}

	//****读取文件头信息
	int buff;
	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//读取前4个字节 文件代码9994
	buff = Function::OnChangeByteOrder(buff);
	if (buff != 9994) {
		file.close();
		std::cerr << "SHP file code doesn't equal 9994." << std::endl;
		return 2;
	}

	file.seekg(5 * 4, std::ios::cur);//skip 20bits

	file.read(reinterpret_cast<char*>(&fileLen), sizeof(fileLen));//读取4字节 文件长度
	fileLen = Function::OnChangeByteOrder(fileLen);

	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//读取4字节 文件版本=1000
	if (buff != 1000) {
		file.close();
		std::cerr << "SHP file version doesn't equal 1000." << std::endl;
		return 3;
	}

	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//读取4字节 几何体类型
	if (buff != 1) {
		file.close();
		std::cerr << "SHPtype isn't point type." << std::endl;
		return 4;
	}

	file.read(reinterpret_cast<char*>(&Xmin), sizeof(Xmin));//读8字节 Xmin
	file.read(reinterpret_cast<char*>(&Ymin), sizeof(Ymin));//8bit
	file.read(reinterpret_cast<char*>(&Xmax), sizeof(Xmax));//8bit
	file.read(reinterpret_cast<char*>(&Ymax), sizeof(Ymax));//8bit

	file.seekg(4 * 8, std::ios::cur);//skip 32bits
	//****读取文件头信息结束

	//****读取实体信息
	int currLen = 100;
	int recordLen;
	int recordType;

	std::vector<int> partLen;
	int numParts, numPoints;
	double pxmin, pymin, pxmax, pymax;
	SimplePolygon pl;
	int c = 0;
	while (currLen < fileLen) {
		//****读取记录头
		file.read(reinterpret_cast<char*>(&buff), sizeof(buff));
		file.read(reinterpret_cast<char*>(&recordLen), sizeof(recordLen));
		recordLen = Function::OnChangeByteOrder(recordLen);
		//****读取记录头结束

		//****记录内容
		file.read(reinterpret_cast<char*>(&recordType), sizeof(recordType));//shapetype
		if (recordType != 5) {
			file.seekg(recordLen - 4);//不符合要求，直接跳到下一个
		}
		else {
			file.read(reinterpret_cast<char*>(&pxmin), sizeof(pxmin));
			file.read(reinterpret_cast<char*>(&pymin), sizeof(pymin));
			file.read(reinterpret_cast<char*>(&pxmax), sizeof(pxmax));
			file.read(reinterpret_cast<char*>(&pymax), sizeof(pymax));
			file.read(reinterpret_cast<char*>(&numParts), sizeof(numParts));
			file.read(reinterpret_cast<char*>(&numPoints), sizeof(numPoints));

			partLen.clear();
			for (int i = 0; i < numParts; i++) {
				file.read(reinterpret_cast<char*>(&buff), sizeof(buff));
				partLen.push_back(buff);
			}

			for (int i = 0; i < numParts - 1; i++){//通过起始位置算长度
				partLen[i] = partLen[i + 1] - partLen[i];
			}
			partLen[numParts - 1] = numPoints - partLen[numParts - 1];

			for (int i = 0; i < numParts; i++){
				pl.x.clear();
				pl.y.clear();
				for (int j = 0; j < partLen[i]; j++)
				{
					double val;
					file.read(reinterpret_cast<char*>(&val), sizeof(val));
					pl.x.push_back(val);
					file.read(reinterpret_cast<char*>(&val), sizeof(val));
					pl.y.push_back(val);
				}
				plgs.push_back(pl);
			}
			currLen += recordLen + 4;
			c++;
		}
		
	}
	file.close();
	return 0;
}



cv::Mat ShapeFilePolygon::generateImage(double div, double l, double r, double b, double t)//像素数，左，右，底，顶
{
	int wid = static_cast<int>((r - l) / div + 1);
	int hei = static_cast<int>((t - b) / div + 1);

	cv::Mat img(hei, wid, CV_8U, cv::Scalar(0));

	std::vector<std::vector<cv::Point>> polygons;

	for (int i = 0; i < plgs.size();i++) {
		const SimplePolygon& pl = plgs.at(i);
		std::vector<cv::Point> pts;

		//将ploygon的点坐标在图像中表示
		for (size_t j = 0; j < pl.x.size(); j++) {
			int x_coord = static_cast<int>((pl.x[j] - l) / div);
			int y_coord = static_cast<int>((t - pl.y[j]) / div);
			pts.push_back(cv::Point(x_coord, y_coord));
		}

		polygons.push_back(pts);
	}

	cv::fillPoly(img, polygons, cv::Scalar(255));//绘制图形

	return img;
}