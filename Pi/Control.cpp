#include <iostream>
#include <wiringPi.h>
#include <pthread.h>
#include "Control.h"

using namespace std;

Motor::Motor()
{
	if (wiringPiSetup() != 0)
	{
		cout << "wiringPi set up error" << endl;
	}
	pinMode(pitchPin1, OUTPUT);
	pinMode(pitchPin2, OUTPUT);
	pinMode(yawPin, OUTPUT);
	
	adjustment = Angle2Time(10);  // 每次舵机调整10°
}


void *Motor::pitchAdjust1(void *args)
{
	digitalWrite(motor.pitchPin1, HIGH);
	delayMicroseconds(motor.highLevelTime[1]);
	digitalWrite(motor.pitchPin1, LOW);
	delayMicroseconds(20000 - motor.highLevelTime[1]);
	pthread_exit(NULL);
}


void *Motor::pitchAdjust2(void *args)
{
	digitalWrite(motor.pitchPin2, HIGH);
	delayMicroseconds(motor.highLevelTime[2]);
	digitalWrite(motor.pitchPin2, LOW);
	delayMicroseconds(20000 - motor.highLevelTime[2]);
	pthread_exit(NULL);
}


void *Motor::yawAdjust(void *args)
{
	digitalWrite(motor.yawPin, HIGH);
	delayMicroseconds(motor.highLevelTime[3]);
	digitalWrite(motor.yawPin, LOW);
	delayMicroseconds(20000 - motor.highLevelTime[3]);
	pthread_exit(NULL);
}


// 将调整角转化为高电平时间调整量
int Motor::Angle2Time(float ang)
{
	return (int) (2000 / 180 * ang);
}


// 根据识别结果进行姿态调整
void Motor::adjust(direction dir)
{
	switch (dir)
	{
	case STRAIGHT:  // 直线冲刺
		highLevelTime[1] = 1500;  // pitch1（左翼）
		highLevelTime[2] = 1500;  // pitch2（右翼）
		highLevelTime[3] = 1500;  // yaw（上翼）
		break;

	case UP:  // 向上飞。pitch1逆时针，pitch2顺时针，yaw不变
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500;
		break;

	case DOWN:  // 向下飞。pitch1顺时针，pitch2逆时针，yaw不变
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500;
		break;

	case LEFT:  // 向左飞。pitch1, pitch2不变，yaw顺时针
		highLevelTime[1] = 1500;
		highLevelTime[2] = 1500;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT:  // 向右飞。pitch1, pitch2不变，yaw逆时针
		highLevelTime[1] = 1500;
		highLevelTime[2] = 1500;
		highLevelTime[3] = 1500 - adjustment;
		break;

	case LEFT_UP:  // 向左上飞。pitch1逆时针，pitch2顺时针，yaw顺时针
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT_UP:  // 向右上飞。pitch1逆时针，pitch2顺时针，yaw逆时针
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500 - adjustment;
		break;

	case LEFT_DOWN:  // 向左下飞。pitch1顺时针，pitch2逆时针，yaw顺时针
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT_DOWN:  // 向右下飞。pitch1顺时针，pitch2逆时针，yaw逆时针
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500 - adjustment;
		break;

	default:
		break;
	}

	/***舵机控制***/
	// 创建pitch1、pitch2、yaw三个线程
	pthread_t pitch1, pitch2, yaw;
	int pitchThread1 = pthread_create(&pitch1, NULL, pitchAdjust1, NULL);
	int pitchThread2 = pthread_create(&pitch2, NULL, pitchAdjust2, NULL);
	int yawThread = pthread_create(&yaw, NULL, yawAdjust, NULL);

	//if (pitchThread1 != 0)
	//{
	//	cout << "pitchThread1 creat error" << endl;
	//}
	//if (pitchThread2 != 0)
	//{
	//	cout << "pitchThread2 creat error" << endl;
	//}
	//if (yawThread != 0)
	//{
	//	cout << "yawThread creat error" << endl;
	//}
}


