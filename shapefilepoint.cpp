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

	//****��ȡ�ļ�ͷ��Ϣ
	int buff;
	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//��ȡǰ4���ֽ� �ļ�����9994
	buff = Function::OnChangeByteOrder(buff);
	if (buff != 9994) {
		file.close();
		std::cerr << "SHP file code doesn't equal 9994." << std::endl;
		return 2;
	}

	file.seekg(5 * 4, std::ios::cur);//skip 20bits

	file.read(reinterpret_cast<char*>(&fileLen), sizeof(fileLen));//��ȡ4�ֽ� �ļ�����
	fileLen = Function::OnChangeByteOrder(fileLen);

	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//��ȡ4�ֽ� �ļ��汾=1000
	if (buff != 1000) {
		file.close();
		std::cerr << "SHP file version doesn't equal 1000." << std::endl;
		return 3;
	}

	file.read(reinterpret_cast<char*>(&buff), sizeof(buff));//��ȡ4�ֽ� ����������
	if (buff != 1) {
		file.close();
		std::cerr << "SHPtype isn't point type." << std::endl;
		return 4;
	}

	file.read(reinterpret_cast<char*>(&Xmin), sizeof(Xmin));//��8�ֽ� Xmin
	file.read(reinterpret_cast<char*>(&Ymin), sizeof(Ymin));//8bit
	file.read(reinterpret_cast<char*>(&Xmax), sizeof(Xmax));//8bit
	file.read(reinterpret_cast<char*>(&Ymax), sizeof(Ymax));//8bit

	file.seekg(4 * 8, std::ios::cur);//skip 32bits
	//****��ȡ�ļ�ͷ��Ϣ����

	//****��ȡʵ����Ϣ
	int currLen = 100;
	int recordLen;
	int recordType;
	SimplePoint pt;
	int c = 0;

	while (fileLen < currLen) {
		file.read(reinterpret_cast<char*>(&buff), sizeof(buff));
		file.read(reinterpret_cast<char*>(&recordLen), sizeof(recordLen));
		recordLen = Function::OnChangeByteOrder(recordLen);
		//****��ȡ��¼ͷ

		file.read(reinterpret_cast<char*>(&recordType), sizeof(recordType));
		if (recordType != 1) {
			file.seekg(recordLen - 4);//������Ҫ��ֱ��������һ����
		}
		else {
			file.read(reinterpret_cast<char*>(&pt.x), sizeof(pt.x));
			file.read(reinterpret_cast<char*>(&pt.y), sizeof(pt.y));
			points.push_back(pt);
		}
		currLen += recordLen + 4;
		c++;
		//****��ȡ�ռ�����
	}
	file.close();
	return 0;
}




