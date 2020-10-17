#pragma once
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "Detect.h"
using namespace std;


class Motor
{
public:
	void adjust(direction dir);  // 根据识别结果进行姿态调整

private:
	int pitchPin1 = 21;  // pitchPin为与左右翼板相连的舵机引脚
	int pitchPin2 = 22;
	int yawPin = 23;  // yawPin为与上翼板相连的舵机引脚
	int highLevelTime[3] = { 1500, 1500, 1500 };  // 当前舵机信号的高电平时间，初始化时间为1500μs，对应中点位置。
	int adjustment;  // 调整一次姿态，舵机高电平时间的调整量

	void *pitchAdjust1(void *args);
	void *pitchAdjust2(void *args);
	void *yawAdjust(void *args);
	int Angle2Time(float ang);  // 将调整角转化为高电平时间调整量
};


#endif // !_CONTROL_H_