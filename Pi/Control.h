#pragma once
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "Detect.h"
using namespace std;


class Motor
{
public:
	void adjust(direction dir);  // ����ʶ����������̬����

private:
	int pitchPin1 = 21;  // pitchPinΪ��������������Ķ������
	int pitchPin2 = 22;
	int yawPin = 23;  // yawPinΪ������������Ķ������
	int highLevelTime[3] = { 1500, 1500, 1500 };  // ��ǰ����źŵĸߵ�ƽʱ�䣬��ʼ��ʱ��Ϊ1500��s����Ӧ�е�λ�á�
	int adjustment;  // ����һ����̬������ߵ�ƽʱ��ĵ�����

	void *pitchAdjust1(void *args);
	void *pitchAdjust2(void *args);
	void *yawAdjust(void *args);
	int Angle2Time(float ang);  // ��������ת��Ϊ�ߵ�ƽʱ�������
};


#endif // !_CONTROL_H_