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
	
	adjustment = Angle2Time(10);  // ÿ�ζ������10��
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


// ��������ת��Ϊ�ߵ�ƽʱ�������
int Motor::Angle2Time(float ang)
{
	return (int) (2000 / 180 * ang);
}


// ����ʶ����������̬����
void Motor::adjust(direction dir)
{
	switch (dir)
	{
	case STRAIGHT:  // ֱ�߳��
		highLevelTime[1] = 1500;  // pitch1������
		highLevelTime[2] = 1500;  // pitch2������
		highLevelTime[3] = 1500;  // yaw������
		break;

	case UP:  // ���Ϸɡ�pitch1��ʱ�룬pitch2˳ʱ�룬yaw����
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500;
		break;

	case DOWN:  // ���·ɡ�pitch1˳ʱ�룬pitch2��ʱ�룬yaw����
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500;
		break;

	case LEFT:  // ����ɡ�pitch1, pitch2���䣬yaw˳ʱ��
		highLevelTime[1] = 1500;
		highLevelTime[2] = 1500;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT:  // ���ҷɡ�pitch1, pitch2���䣬yaw��ʱ��
		highLevelTime[1] = 1500;
		highLevelTime[2] = 1500;
		highLevelTime[3] = 1500 - adjustment;
		break;

	case LEFT_UP:  // �����Ϸɡ�pitch1��ʱ�룬pitch2˳ʱ�룬yaw˳ʱ��
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT_UP:  // �����Ϸɡ�pitch1��ʱ�룬pitch2˳ʱ�룬yaw��ʱ��
		highLevelTime[1] = 1500 - adjustment;
		highLevelTime[2] = 1500 + adjustment;
		highLevelTime[3] = 1500 - adjustment;
		break;

	case LEFT_DOWN:  // �����·ɡ�pitch1˳ʱ�룬pitch2��ʱ�룬yaw˳ʱ��
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500 + adjustment;
		break;

	case RIGHT_DOWN:  // �����·ɡ�pitch1˳ʱ�룬pitch2��ʱ�룬yaw��ʱ��
		highLevelTime[1] = 1500 + adjustment;
		highLevelTime[2] = 1500 - adjustment;
		highLevelTime[3] = 1500 - adjustment;
		break;

	default:
		break;
	}

	/***�������***/
	// ����pitch1��pitch2��yaw�����߳�
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


