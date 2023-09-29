#include "solver.h"

Solver::Solver()
{
	w = 0;
	h = 0;
	currT = 0;
	deltaT = 0;
	rainT = 0;
	reportT = 0;
	totalT = 0;
	deltaX = 0;
	edgeNum = 0;
	cellNum = 0;
	edges = NULL;
	cells = NULL;
	IDs = NULL;
}

Solver::~Solver()
{
	if (edges) {
		delete[] edges;
		edges = NULL;
	}
	if (cells) {
		delete[] cells;
		cells = NULL;
	}
	if (IDs) {
		delete[] IDs[0];
		delete[] IDs;
		IDs = NULL;
	}
}


void Solver::addEdge(int cellID, int edgeID, bool isNeg)
{
	int ind = cells[cellID].edgeNum;
	cells[cellID].edges[ind] = edgeID;
	cells[cellID].isNeg[ind] = isNeg;
	cells[cellID].edgeNum = ind + 1;
}

void Solver::divideGrid()
{
	if (edges) {
		delete[] edges;
		edges = NULL;
	}
	if (cells) {
		delete[] cells;
		cells = NULL;
	}
	if (IDs) {
		for (int i = 0; i < w; i++) {
			delete[] IDs[i];
		}
		delete[] IDs;
		IDs = NULL;
	}

	edgeNum = w * (h - 1) + h * (w - 1);
	edges = new Edge[edgeNum];
	cellNum = w * h;
	cells = new Cell[cellNum];
	IDs = new int* [w];
	for (int x = 0; x < w; x++) {
		IDs[x] = new int[h];
	}
	
	int c = 0;
	Array2D<char>& isBuilding = GlobalData::isBuilding;
	for (int y = 0; y < w; y++) {
		for (int x = 0; x < h; x++) {
			if(GlobalData::building==1){
				if (!isBuilding[x][y]) {
					IDs[x][y] = c;
					cells[c].x = x;
					cells[c].y = y;
					cells[c].z = GlobalData::z[x][y];
					cells[c].edgeNum = 0;
					cells[c].d = 0;
					cells[c].totalRain = rainfall;
					cells[c].drainID = -1;
					cells[c].drain = 0;
					c++;
				}
				else {
					IDs[x][y] = -1;
				}
			}
			else {
				IDs[x][y] = -1;
			}
		}
	}
	cellNum = c;

	c = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w-1; x++) {
			int cellW = IDs[x][y];
			int cellE = IDs[x + 1][y];
			if (cellW < 0 || cellE < 0) {
				continue;
			}
			edges[c].cellC = cellW;
			edges[c].cellE = cellE;
			edges[c].cellN = y < h - 1 ? IDs[x][y + 1] : -1;
			edges[c].cellS = y > 0 ? IDs[x][y - 1] : -1;
			edges[c].cellNE = y < h - 1 ? IDs[x + 1][y + 1] : -1;
			edges[c].cellSE = y > 0 ? IDs[x + 1][y - 1] : -1;
			addEdge(cellW, c, true);
			addEdge(cellE, c, false);
			c++;
		}
	}
	for (int y = 0; y < h - 1; y++) {
		for (int x = 0; x < w; x++) {
			int cellS = IDs[x][y];
			int cellN = IDs[x][y + 1];
			if (cellS < 0 || cellN < 0) {
				continue;
			}
			edges[c].cellC = cellS;
			edges[c].cellE = cellN;
			edges[c].cellN = x > 0 ? IDs[x - 1][y] : -1;
			edges[c].cellNE = x > 0 ? IDs[x - 1][y + 1] : -1;
			edges[c].cellS = x < w - 1 ? IDs[x + 1][y] : -1;
			edges[c].cellSE = x < w - 1 ? IDs[x + 1][y + 1] : -1;
			addEdge(cellS, c, true);
			addEdge(cellN, c, false);
			c++;
		}
	}
	edgeNum = c;

	for (int i = 0; i < edgeNum; i++) {
		Edge* edge = &edges[i];
		if (edge->cellS < 0) {
			if (edge->cellN < 0) {
				edge->typeC = 0;//上下0
			}
			else {
				edge->typeC = 1;//上1下0
			}
		}
		else {
			if (edge->cellN < 0) {
				edge->typeC = 2;//上0下1
			}
			else {
				edge->typeC = 3;//上下1
			}
		}

		if (edge->cellSE < 0) {
			if (edge->cellNE < 0) {
				edge->typeE = 0;
			}
			else {
				edge->typeE = 1;
			}
		}
		else {
			if (edge->cellNE < 0) {
				edge->typeE = 2;
			}
			else {
				edge->typeE = 3;
			}
		}
	}
}

void Solver::calcDrains()
{
	for (int i=0; i < GlobalData::drainList.size(); i++) {
		Drain& dr = GlobalData::drainList[i];
		int tx = dr.x;
		int ty = dr.y;
		if (tx >= 0 && ty >= 0 && tx <= w - 1 && ty <= h - 1) {
			int tID = IDs[tx][ty];//非建筑单元格编号
			if (tID >= 0) {//建筑单元格为-1，即若不是建筑
				if (cells[tID].drainID < 0) {
					cells[tID].drain = GlobalData::drainC * dr.areaHole;
					cells[tID].drainID = i;
					dr.tID = tID;
				}
				else {

				}
			}
		}
	}
}

void Solver::calcBuildingRain()
{
	int* buildingNo = new int[w * h];//建筑物编号
	for (int i = 0; i < w * h; i++) {
		buildingNo[i] = -1;
	}
	int currNo = 0;//当前建筑编号
	int* buildingNNum = new int[w * h];//每个建筑的单元格数量
	Array2D<char>& isBuilding = GlobalData::isBuilding;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (isBuilding[x][y] && buildingNo[y * w + x] < 0) {
				buildingNNum[currNo] = 1;
				buildingNo[y * w + x] = currNo;
				std::queue<Point<int>> spreadN;
				spreadN.push(Point<int>(x, y));

				while (!spreadN.empty()) {
					Point pt = spreadN.front();
					int tx = pt.x;
					int ty = pt.y;
					//查询周围单元格建筑
					int dx[] = { -1,1,0,0 };
					int dy[] = { 0,0,-1,1 };
					for (int i = 0; i < 4; i++) {
						int nx = tx + dx[i];
						int ny = ty + dy[i];
						if (nx >= 0 && nx < w && ny >= 0 && ny < h 
							&& isBuilding[nx][ny] && buildingNo[nx+ny*w]) {
							buildingNNum[currNo]++;
							buildingNo[nx+ny * w] = currNo;
							spreadN.push(Point<int>(nx, ny));
						}
					}
				}
				currNo++;
			}
		}
	}

	int* buildingNEdge = new int[currNo];
	memset(buildingNEdge, 0, sizeof(int) * currNo);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (buildingNo[y * w + x] >= 0) {
				int bno = buildingNo[y * w + x];//获取建筑物编号
				if(x > 0 && buildingNo[x + y * w - 1] < 0){
					buildingNEdge[bno]++;
				}
				if (x < w - 1 && buildingNo[x + y * w + 1] < 0) {
					buildingNEdge[bno]++;
				}
				if (y > 0 && buildingNo[(y - 1) * w + x] < 0) {
					buildingNEdge[bno]++;
				}
				if (y < h - 1 && buildingNo[(y + 1) * w + x] < 0) {
					buildingNEdge[bno]++;
				}
			}
		}
	}

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (buildingNo[y * w + x] < 0) {//不在建筑格子上
				Cell* cell = &cells[IDs[x][y]];
				if (x > 0 && buildingNo[y * w + x - 1] >= 0) {
					int bno = buildingNo[y * w + x - 1];
					cell->totalRain += static_cast<double>(buildingNNum[bno] / buildingNEdge[bno]) * rainfall;
				}
				if (x < w - 1 && buildingNo[y * w + x + 1] >= 0) {
					int bno = buildingNo[y * w + x + 1];
					cell->totalRain += static_cast<double>(buildingNNum[bno] / buildingNEdge[bno]) * rainfall;
				}
				if (y > 0 && buildingNo[(y - 1) * w + x] >= 0) {
					int bno = buildingNo[(y - 1) * w + x];
					cell->totalRain += static_cast<double>(buildingNNum[bno] / buildingNEdge[bno]) * rainfall;
				}
				if (y < h - 1 && buildingNo[(y + 1) * w + x] >= 0) {
					int bno = buildingNo[(y + 1) * w + x];
					cell->totalRain += static_cast<double>(buildingNNum[bno] / buildingNEdge[bno]) * rainfall;
				}
			}
		}
	}

	delete[] buildingNo;
	delete[] buildingNNum;
	delete[] buildingNEdge;
}



int Solver::init()
{
	currT = 0;
	reportT = GlobalData::reportTime;
	totalT = static_cast<int>(GlobalData::totalTime / reportT) * reportT;
	rainT = GlobalData::rainTime;
	w = GlobalData::w;
	h = GlobalData::h;
	deltaX = GlobalData::deltaX;
	deltaT = 0.01 * GlobalData::courant;
	rainfall = GlobalData::rainfallIntensity / 1000 / 3600;
	if (!GlobalData::z.get_wid()) {
		return 1;
	}
	divideGrid();
	if(GlobalData::building==1)
	{
		calcBuildingRain();
	}
	if(GlobalData::drain==1)
	{
		calcDrains();
	}
	for (int i = 0; i < SWMMInterface::drainCount; i++) {
		SWMMInterface::surcharge[i] = false;
		SWMMInterface::depthCache[i] = 0;
	}
	return 0;
 }

int Solver::run(const std::string& dirPath)
{
	int sec = static_cast<int>(currT / reportT) * reportT;
	if (currT == 0) {
		std::filesystem::create_directory(dirPath + "/output");

		for (const auto& entry : std::filesystem::directory_iterator(dirPath + "/output")) {
			std::string filename = entry.path().filename().string();
			if (filename.find(".dat") != std::string::npos) {
				std::filesystem::remove(dirPath + "/output/" + filename);
			}
		}

		outputDepthData(dirPath + "/output/00000.dat");
	}

	while (currT < totalT) {
		calcStep();

		if (currT >= sec + reportT) {
			sec += reportT;
			std::ostringstream nameStream;
			nameStream << std::setw(5) << std::setfill('0') << sec;
			std::string name = nameStream.str();
			outputDepthData(dirPath + "/output/" + name + ".dat");
		}
	}
	return 0;
}

int Solver::outputDepthData(const std::string& path)
{
	if (!cells) {
		return 1;
	}

	if (GlobalData::outputMode == OUTPUT_ASCII) {
		std::ofstream ofs(path);
		if (!ofs.is_open()) {
			return 2;
		}
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int tID = IDs[x][y];
				if (tID >= 0) {
					ofs << cells[tID].d;
				}
				else {
					ofs << -1.0;
				}
				ofs << "";
			}
			ofs << "\n";
		}
		ofs.close();
	}
	else if (GlobalData::outputMode == OUTPUT_BINARY) {
		std::ofstream ofs(path, std::ios::binary);
		if (!ofs.is_open()) {
			return 2;
		}
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int tID = IDs[x][y];
				float value = (tID >= 0) ? cells[tID].d : -1.0;
				ofs.write(reinterpret_cast<char*>(&value), sizeof(float));
			}
		}
		ofs.close();
	}
	
	return 0;
}

int Solver::calcStep()
{
	double maxV = 0;
	for (int i = 0; i < cellNum; i++) {
		Cell* cell = &cells[i];
		cell->h = cell->d + cell->z;
	}
	if(GlobalData::drain==1)
	{
		for (int i = 0; i < GlobalData::drainList.size(); i++) {
			const Drain& dr = GlobalData::drainList.at(i);
			int tID = dr.tID;
			if (tID >= 0) {
				if (SWMMInterface::surcharge[i]) {
					SWMMInterface::depthCache[i] = cells[tID].h - dr.elev;
				}
			}
		}
		SWMMInterface::setDepths();
	}

	for (int i = 0; i < edgeNum; i++) {
		Edge* edge = &edges[i];
		double hI = cells[edge->cellC].h;
		double hJ = cells[edge->cellE].h;
		double dhdx = (hJ - hI) / deltaX;
		double d = std::max(hI, hJ) - std::max(cells[edge->cellC].z, cells[edge->cellE].z);
		if (d <= 0 || dhdx == 0) {
			edge->Q = 0;
		}
		else {
			//南北边界
			double dhdyC = 0;
			switch (edge->typeC) {
			case 1:
				dhdyC = (cells[edge->cellN].h - cells[edge->cellC].h) / deltaX / 2;
				break;
			case 2:
				dhdyC = (cells[edge->cellC].h - cells[edge->cellS].h) / deltaX / 2;
				break;
			case 3:
				dhdyC = (cells[edge->cellN].h - cells[edge->cellS].h) / deltaX / 4;
				break;
			}
			double dhdyE = 0;
			switch (edge->typeE) {
			case 1:
				dhdyE = (cells[edge->cellNE].h - cells[edge->cellE].h) / deltaX / 2;
				break;
			case 2:
				dhdyE = (cells[edge->cellE].h - cells[edge->cellSE].h) / deltaX / 2;
				break;
			case 3:
				dhdyE = (cells[edge->cellNE].h - cells[edge->cellSE].h) / deltaX / 4;
				break;
			}
			double dhdy = dhdyC + dhdyE;
			double dh = sqrt(dhdx * dhdx + dhdy * dhdy);
			double v = sqrt(dh) * pow(d, 2.0 / 3.0) / GlobalData::manningN;//(3-14)
			if (v != v) {
				edge->Q = 0;
			}
			else {
				if (v > maxV) {
					maxV = v;
				}
				double vx = -dhdx / dh * v;//(3-20)
				edge->Q = vx * d * deltaX;//(3-21)
			}
		}
	}

	double maxD = 0;
	for (int i = 0; i < cellNum; i++) {
		Cell* cell = &cells[i];
		if (cell->d > maxD) {
			maxD = cell->d;
		}
		double totalQ = 0;
		for (int j = 0; j < cell->edgeNum; j++) {
			if (cell->isNeg[j]) {
				totalQ -= edges[cell->edges[j]].Q;
			}
			else {
				totalQ += edges[cell->edges[j]].Q;
			}
		}
		cell->dddt = totalQ / deltaX / deltaX;//水深变化速率
	}
	deltaT = deltaX / (maxV + sqrt(GRAVITY_CONSTANT * maxD)) * GlobalData::courant;
	if (deltaT > 1) {
		deltaT = 1;
	}
	for (int i = 0; i < edgeNum; i++) {
		Edge* edge = &edges[i];
		Cell* cI = &cells[edge->cellC];
		Cell* cJ = &cells[edge->cellE];
		double dt3 = deltaT;
		double dt2 = (cI->d + cI->z - cJ->d - cJ->z) / (cJ->dddt - cI->dddt);//更新时间步长
		if (dt2 > 0 && dt2 < deltaT) {
			dt3 = dt2;
		}
		edge->Qdt = edge->Q * dt3;
	}
	for (int i = 0; i < cellNum; i++) {
		Cell* cell = &cells[i];
		double totalQ = 0;
		for (int j = 0; j < cell->edgeNum; j++) {
			if (cell->isNeg[j]) {
				totalQ -= edges[cell->edges[j]].Qdt;
			}
			else {
				totalQ += edges[cell->edges[j]].Qdt;
			}
		}
		cell->d += totalQ / deltaX / deltaX;//(3-23)
	}
	for (int i = 0; i < cellNum; i++) {
		Cell* cell = &cells[i];
		double srcTerm = 0;
		if (currT < rainT) {
			srcTerm = cell->totalRain;//源项
		}
		//Infiltration (ignored)
		cell->d += srcTerm * deltaT;
		if (cell->d < 0) {
			//cell->rain-=cell->d;
			cell->d = 0;
		}
	}

	if(GlobalData::drain)
	{
		SWMMInterface::routingStep(deltaT);
		SWMMInterface::getDepths();

		for (int i = 0; i < cellNum; i++) {
			Cell* cell = &cells[i];
			int dID = cell->drainID;
			if (dID >= 0) {
				const Drain& dr = GlobalData::drainList.at(dID);
				bool& surcharge = SWMMInterface::surcharge[dID];
				double& dNew = SWMMInterface::depthCacheOut[dID];
				double& dOld = SWMMInterface::depthCache[dID];
				if (surcharge) {
					dNew -= dOld + dr.elev - cell->z;
				}
				double hNew = dNew + dr.elev;
				double VDrain = 0;
				if (hNew >= cell->z) {
					VDrain = -(hNew - cell->z) * dr.area;
					surcharge = true;
				}
				else {
					VDrain = cell->drain * sqrt(2 * GRAVITY_CONSTANT * cell->d) * deltaT;
					double VDLim = cell->d * deltaX * deltaX;
					if (VDrain > VDLim) {
						VDrain = VDLim;
					}
					dNew += VDrain / dr.area;
					hNew += VDrain / dr.area;
					if (hNew >= cell->z) {
						VDrain -= (hNew - cell->z) * dr.area;
						surcharge = true;
					}
					else {
						surcharge = false;
					}
				}
				cell->d -= VDrain / deltaX / deltaX;
				if (!surcharge) {
					dOld = dNew;
				}
			}
		}
	}
	currT += deltaT;
	return 0;
}