#pragma once

class Function
{
public:
	static int OnChangeByteOrder(int indata);
};

int Function::OnChangeByteOrder(int indata)//将big数位序转换为little数位序
{
	unsigned int val = static_cast<unsigned int>(indata); // 转换为无符号整数以确保正数处理

	// 将整数按字节顺序进行倒序
	val = ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) |
		((val << 8) & 0xFF0000) | ((val << 24) & 0xFF000000);

	return static_cast<int>(val); // 转换回有符号整数
}