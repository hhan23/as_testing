#include "shapefilepoint.h"
#include "function.h"

int ShapeFilePoint::loadSHP(const std::string& path)
{
	points.clear();
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
	SimplePoint pt;
	int c = 0;

	while (fileLen < currLen) {
		file.read(reinterpret_cast<char*>(&buff), sizeof(buff));
		file.read(reinterpret_cast<char*>(&recordLen), sizeof(recordLen));
		recordLen = Function::OnChangeByteOrder(recordLen);
		//****读取记录头

		file.read(reinterpret_cast<char*>(&recordType), sizeof(recordType));
		if (recordType != 1) {
			file.seekg(recordLen - 4);//不符合要求，直接跳到下一个点
		}
		else {
			file.read(reinterpret_cast<char*>(&pt.x), sizeof(pt.x));
			file.read(reinterpret_cast<char*>(&pt.y), sizeof(pt.y));
			points.push_back(pt);
		}
		currLen += recordLen + 4;
		c++;
		//****读取空间数据
	}
	file.close();
	return 0;
}




