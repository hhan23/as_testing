#include "globaldata.h"
#include "swmminterface.h"

/*GlobalData::GlobalData()
{
	courant = 0.1;
	deltaX = 1.0;
	totalTime = 10;
	rainTime = 0;
	outputStep = 1;
	rainfallIntensity = 0;
	manningN = 0.01012584304314985349226867976264;
	epsilon = 0.0001;
	drainA = 0.17;
	drainC = 0.65;
	//rainfallIntensity=52.239;//5year
	//rainfallIntensity=71.156;//20year
	//rainfallIntensity=133.803;//100year
	//rainTime = 3600;
	//totalTime = rainTime + 1800;
	//outputStep = 60;
}*/

void GlobalData::genXDamBreak(int tw, double dHigh, double dLow)
{
	w = tw;
	h = 1;
	isBuilding.reAlloc(w, h);
	z.reAlloc(w, h);
	d0.reAlloc(w, h);
	isBuilding.fill(0);
	z.fill(0.0);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			d0[x][y] = (x < w / 2 ? dHigh : dLow);
		}
	}
}

void GlobalData::genYDamBreak(int th, double dHigh, double dLow)
{
	w = 1;
	h = th;
	isBuilding.reAlloc(w, h);
	z.reAlloc(w, h);
	d0.reAlloc(w, h);
	isBuilding.fill(0);
	z.fill(0.0);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			d0[x][y] = (y < h / 2 ? dHigh : dLow);
		}
	}
}

void GlobalData::genXDamBreakSlope(int tw, double dHigh, double slope)
{
	w = tw;
	h = 1;
	isBuilding.reAlloc(w, h);
	z.reAlloc(w, h);
	d0.reAlloc(w, h);
	isBuilding.fill(0);
	z.fill(0.0);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			z[x][y] = (w / 2 - 1 - x) * slope * deltaX;
			//d0[x][y]=dHigh-elev[x][y];
			d0[x][y] = (x < w / 2 ? dHigh - z[x][y] : 0);
		}
	}
	drains.clear();
	drainA = 0.17;
	drainC = 0.65;
}

void GlobalData::genSquareDamBreak(int ta, double dHigh, double dLow)
{
	w = ta;
	h = ta;
	isBuilding.reAlloc(w, h);
	//elev.reAlloc(wid,hei);
	d0.reAlloc(w, h);
	isBuilding.fill(0);
	//elev.fill(0.0);
	for (int x = 0; x < ta; x++) {
		for (int y = 0; y < ta; y++) {
			d0[x][y] = (x + y < ta ? dHigh : dLow);
		}
	}
}

void GlobalData::genRandomBox(int tw, int th, double zHigh, double zLow, double elevW)
{
	srand(time(0));
	w = tw;
	h = th;
	isBuilding.reAlloc(w, h);
	z.reAlloc(w, h);
	d0.reAlloc(w, h);
	isBuilding.fill(0);
	//elev.fill(0.0);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			z[x][y] = zLow + (((double)rand()) / RAND_MAX) * (zHigh - zLow);
			d0[x][y] = elevW - z[x][y];
		}
	}
}

int GlobalData::initalinpfile()
{
	std::ofstream file("swmm.inp");
	if (!file.is_open()) {
		throw("Can't open the swmm.inp file for writing");
	}
	file << "[TITLE]\n";
	file << "UPFLOOD INPUTFILE\n";
	file << "\n";
	file << "[OPTIONS]\n";
	file << "FLOW_UNITS			CFS\n";


	file << "\n[JUNCTION]\n";
	file << "";
}

void GlobalData::inital(ASCIIGridFile& gf)
{
	x1 = gf.LEFT_X;
	x2 = gf.RIGHT_X;
	y1 = gf.DOWN_Y;
	y2 = gf.UP_Y;
	w = gf.ncols;
	h = gf.nrows;
	deltaX = gf.PixelSizeX;
	z.reAlloc(w, h);
	d0.reAlloc(w, h);
	isBuilding.reAlloc(w, h);
	isBuilding.fill(0);
	int baseX = (x1 - gf.LEFT_X) / deltaX;
	int baseY = (gf.UP_Y - y1) / deltaX;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			z[x][y] = gf.map[baseX + x][baseY - y - 1];
			d0[x][y] = 0.0;
		}
	}
}

void GlobalData::freshBuildings(ASCIIGridFile& gf) 
{
	int baseX = (x1 - gf.LEFT_X) / deltaX;
	int baseY = (gf.UP_Y - y1) / deltaX;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			if (gf.map[baseX + x][baseY - 1 - y] >= 0) {
				isBuilding[x][y] = 1;
			}
		}
	}
}

void GlobalData::freshBuildings(ShapeFilePolygon& sfpg)
{
	cv::Mat img1 = sfpg.generateImage(deltaX, x1, x2, y1, y2);
	isBuilding.fill(0);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			cv::Scalar pixel = img1.at<uchar>(y, x);
			if (pixel.val[0] != 0) {
				isBuilding[x][y] = 1;
			}
		}
	}
}

void GlobalData::freshDrains(ShapeFilePoint& sfp)
{
	std::vector<Point<double>> drain2;
	int num = sfp.points.size();
	int tx, ty;
	for (int i=0; i < num; i++) {
		tx = static_cast<int>((sfp.points[i].x - x1)) / deltaX;
		ty = static_cast<int>((sfp.points[i].y - y1)) / deltaX;
		if (tx >= 0 && tx < w && ty >= 0 && ty < h) {
			drain2.push_back(Point<double>(tx, ty));
		}
	}
	drains = drain2;
}

int GlobalData::freshDrains(char* f1, char* f2, char* f3)
{
	SWMMInterface::init(f1, f2, f3);
	drainList.resize(SWMMInterface::drainCount);
	std::ifstream file(f1);
	std::string line;

	if (!file.is_open()) {
		return -1;
	}
	while (std::getline(file, line)) {
		if (line == "[STORAGE]") {
			break;
		}
	}
	std::getline(file, line);
	std::getline(file, line);
	while (std::getline(file, line)) {
		if (line.empty()) {
			break;
		}
		std::istringstream iss(line);
		std::vector<std::string> tokens;
		for (std::string token; iss >> token;) {
			tokens.push_back(token);
		}
		if (tokens.size() < 8) {
			break;
		}
		int ind = SWMMInterface::findDrain(tokens[0].c_str());
		if (ind < 0) {
			file.close();
			return -1;
		}
		drainList[ind].elev = std::stod(tokens[1]);
		drainList[ind].areaHole = std::stod(tokens[6]);
		drainList[ind].area = std::stod(tokens[7]);
		drainList[ind].tID = -1;
	}
	// 读取至 "[COORDINATES]"
	while (std::getline(file, line)) {
		if (line == "[COORDINATES]") {
			break;
		}
	}

	std::getline(file, line);
	std::getline(file, line);

	// 处理坐标
	while (std::getline(file, line)) {
		if (line.empty()) {
			break;
		}

		std::istringstream iss(line);
		std::vector<std::string> tokens;

		for (std::string token; iss >> token;) {
			tokens.push_back(token);
		}

		if (tokens.size() < 3) {
			break;
		}

		int ind = SWMMInterface::findDrain(tokens[0].c_str());
		if (ind >= 0) {
			int tx = (std::stod(tokens[1]) - x1) / deltaX;
			int ty = (std::stod(tokens[2]) - y1) / deltaX;
			drainList[ind].x = tx;
			drainList[ind].y = ty;
		}
	}

	file.close();
	return 0;
}

void GlobalData::deinit()
{
	SWMMInterface::deinit();
}