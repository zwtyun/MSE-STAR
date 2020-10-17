#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include "serialPI.h"

#define CMSPAR 0x20
using namespace std;

// �򿪴���
void Serial::openPort()
{
	if ((serialFd = open("/dev/ttyAMA0", O_RDONLY | O_NOCTTY | O_NONBLOCK)) != 0)    //�򿪴��ڣ�ֻ��ģʽ��������ģʽ
	{
		cout << "OPEN FAILED" << endl;
		exit(-1);
	}
}


// ���ڳ�ʼ��
void Serial::initPort()
{
	struct termios serialAttr;
	/*����������д��ڲ������ã�������������ںŵȳ���������صĳ�����Ϣ*/
	if (tcgetattr(serialFd, &serialAttr) != 0)
	{
		cout << "GET ATTR FAILED" << endl;
		exit(-1);
	}
	//bzero(&serialAttr, sizeof(serialAttr));  // ����
	
	/*����c_cflag����ģʽ*/
	serialAttr.c_cflag |= CLOCAL;  // ������ã�modem�Ŀ����߽��ᱻ���ԡ����û�����ã���open()����������ֱ���ز����������modem����ժ��״̬Ϊֹ��
	serialAttr.c_cflag |= CREAD;  // ֻ�������˲��ܽ����ַ����ñ����һ��Ҫ���õġ�
	serialAttr.c_cflag |= CS8;  // ����λΪ8bit
	serialAttr.c_cflag |= PARENB | CMSPAR;  // ����ΪspaceУ�飨У��λΪ0��

	/*���ò�����*/
	cfsetispeed(&serialAttr, B921600);  /**********921600��WT931��Ĭ�ϲ����ʣ�����ʱ���������ݮ�ɵĴ��ڲ�֧����ô�ߵĲ����ʣ������½�*********/
	cfsetospeed(&serialAttr, B921600);

	/*���õȴ�ʱ�����С�����ַ�*/
	serialAttr.c_cc[VTIME] = 0;
	serialAttr.c_cc[VMIN] = 0;

	/*����δ�����ַ�*/
	tcflush(serialFd, TCIFLUSH);

	/*����������*/
	if ((tcsetattr(serialFd, TCSANOW, &serialAttr)) != 0)  // ������������
	{
		cout << "SET ATTR FAILED" << endl;
		exit(-1);
	}

	cout << "Serial initialized successfully!" << endl;
}


// ���������ݣ����������ݻ���õ���ʵ����
void Serial::getData()
{
	ushort usTemp[11] = { 0 };
	short sTemp[22] = { 0 };

	/*��һ������*/
	while (1)
	{
		if (read(serialFd, usTemp, 1) != 0)
		{
			cout << "READ FAILED" << endl;
			exit(-1);
		}

		/*һֱ����ֱ��"0x55 0x50"����*/
		if (usTemp[0] != 0x55)
			continue;
		read(serialFd, &usTemp[1], 1);
		if (usTemp[1] != 0x50)
			continue;

		/*����"0x55 0x50"���ٶ�31���ֽڣ������к�У��*/
		read(serialFd, &usTemp[2], 9);
		read(serialFd, sTemp, 22);
		ushort usSum;
		short sSum[2] = { 0 };
		for (int i = 0; i < 10; i++)
		{
			usSum += usTemp[i];
			sSum[0] += sTemp[i];
			sSum[1] += sTemp[i + 11];
		}
		if ((usSum != usTemp[10]) || (sSum[0] != sTemp[10]) || (sSum[1] != sTemp[21]))
			continue;

		/*�����У�飬��д��*/
		memcpy(rawTime, &usTemp[5], 5);
		memcpy(rawAcc, &sTemp[2], 6);
		memcpy(rawAngle, &sTemp[13], 6);
		break;
	}

	/*�������ݴ����õ�ʵ�ʵ�ʱ�䡢���ٶȺ�ŷ����*/
	// ʱ��
	time.hour = rawTime[0];
	time.minute = rawTime[1];
	time.second = rawTime[2];
	time.milisecond = (rawTime[4] << 8) | rawTime[3];

	short temp[3] = { 0 };
	// ���ٶ�
	temp[0] = short(rawAcc[1] << 8 | rawAcc[0]);
	acc[0] = temp[0] / 32768 * 16 * 9.8;  // x��
	temp[1] = short(rawAcc[3] << 8 | rawAcc[2]);
	acc[1] = temp[1] / 32768 * 16 * 9.8;  // y��
	temp[2] = short(rawAcc[5] << 8 | rawAcc[4]);
	acc[2] = temp[3] / 32768 * 16 * 9.8;  // z��
	accSum = abs(acc[0]) + abs(acc[1]) + abs(acc[2]);

	// ŷ����
	temp[0] = short(rawAngle[1] << 8 | rawAngle[0]);
	angle[0] = temp[0] / 32768 * 180;  // x��
	temp[1] = short(rawAngle[3] << 8 | rawAngle[2]);
	angle[1] = temp[1] / 32768 * 180;  // y��
	temp[2] = short(rawAngle[5] << 8 | rawAngle[4]);
	angle[2] = temp[3] / 32768 * 180;  // z��

}


// �رմ���
void Serial::closePort()
{
	tcflush(serialFd, TCIFLUSH);  // ��մ�������
	if (close(serialFd) != 0)  // �رմ���
	{
		cout << "CLOSE FAILED" << endl;
		exit(-1);
	}
}


// �жϷ����Ƿ���
bool Serial::isLaunched()
{
	if (accSum > 5 * 9.8)
		return true;
	else
		return false;
}


// �жϷ��ڼ��ٶ��Ƿ��ȶ�
bool Serial::isSteady()
{
	if ((accSum < 3 * 9.8) && (accSum > 0.8 * 9.8))
		return true;
	else
		return false;
}