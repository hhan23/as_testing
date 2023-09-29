#pragma once

#include <memory.h>

template <typename T> 
class Array2D
{
private:
	int wid, hei;
	T** dat;
public:
	Array2D() :wid(0), hei(0), dat(0)
	{

	}

	Array2D(int twid, int thei) :wid(twid), hei(thei)//dat[wid][hei] [x][y]
	{
		dat = new T* [wid];
		for (int x = 0; x < wid; x++) {
			dat[x] = new T[hei];
		}
	}

	~Array2D()
	{
		cleanup();
	}

	void cleanup() 
	{
		for (int i = 0; i < wid; i++) {
			delete[] dat[i];
		}
		delete[] dat;
	}

	int get_wid()
	{
		return wid;
	}

	int get_hei()
	{
		return hei;
	}

	T* operator [](int i)
	{
		return dat[i];
	}

	void reAlloc(int twid, int thei)
	{
		cleanup();
		wid = twid;
		hei = thei;
		dat = new T * [wid];
		for (int i = 0; i < wid; i++) {
			dat[i] = new T[hei];
		}
	}

	void genMaxMin(T* min, T* max)//iÐÐjÁÐ
	{
		*min = *max = dat[0][0];
		for (int x = 0; x < wid; x++) {
			for (int y = 0; y < hei; y++) {
				if (dat[x][y] < *min) {
					*min = dat[x][y];
				}
				if (dat[x][y] > *max) {
					*max = dat[x][y];
				}
			}
		}
	}

	void fill(T value)
	{
		for (int x = 0; x < wid; x++) {
			for (int y = 0; y < hei; y++) {
				dat[x][y] = value;
			}
		}
	}
};


class Edge
{
public:
	char typeC, typeE;
	int cellC, cellE;
	int cellN, cellNE, cellS, cellSE;
	double Q, Qdt;
};

class Cell
{
public:
	int x, y;
	double z, d, h;
	char edgeNum;
	int edges[4];
	bool isNeg[4];
	double drain;
	double totalRain;
	double dddt;
	int drainID;
};

class Drain
{
public:
	int x, y, tID;
	double areaHole;
	double area;
	double elev;
};

template <typename T>
class Point
{
public:
	T x, y;
};