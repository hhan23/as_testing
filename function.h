#pragma once

class Function
{
public:
	static int OnChangeByteOrder(int indata);
};

int Function::OnChangeByteOrder(int indata)//��big��λ��ת��Ϊlittle��λ��
{
	unsigned int val = static_cast<unsigned int>(indata); // ת��Ϊ�޷���������ȷ����������

	// ���������ֽ�˳����е���
	val = ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) |
		((val << 8) & 0xFF0000) | ((val << 24) & 0xFF000000);

	return static_cast<int>(val); // ת�����з�������
}