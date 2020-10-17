#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include "serialPI.h"

#define CMSPAR 0x20
using namespace std;

// 打开串口
void Serial::openPort()
{
	if ((serialFd = open("/dev/ttyAMA0", O_RDONLY | O_NOCTTY | O_NONBLOCK)) != 0)    //打开串口，只读模式，非阻塞模式
	{
		cout << "OPEN FAILED" << endl;
		exit(-1);
	}
}


// 串口初始化
void Serial::initPort()
{
	struct termios serialAttr;
	/*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
	if (tcgetattr(serialFd, &serialAttr) != 0)
	{
		cout << "GET ATTR FAILED" << endl;
		exit(-1);
	}
	//bzero(&serialAttr, sizeof(serialAttr));  // 清零
	
	/*设置c_cflag控制模式*/
	serialAttr.c_cflag |= CLOCAL;  // 如果设置，modem的控制线将会被忽略。如果没有设置，则open()函数会阻塞直到载波检测线宣告modem处于摘机状态为止。
	serialAttr.c_cflag |= CREAD;  // 只有设置了才能接收字符，该标记是一定要设置的。
	serialAttr.c_cflag |= CS8;  // 数据位为8bit
	serialAttr.c_cflag |= PARENB | CMSPAR;  // 设置为space校验（校验位为0）

	/*设置波特率*/
	cfsetispeed(&serialAttr, B921600);  /**********921600是WT931的默认波特率，调试时如果发现树莓派的串口不支持这么高的波特率，再往下降*********/
	cfsetospeed(&serialAttr, B921600);

	/*设置等待时间和最小接收字符*/
	serialAttr.c_cc[VTIME] = 0;
	serialAttr.c_cc[VMIN] = 0;

	/*处理未接收字符*/
	tcflush(serialFd, TCIFLUSH);

	/*激活新配置*/
	if ((tcsetattr(serialFd, TCSANOW, &serialAttr)) != 0)  // 立即更新配置
	{
		cout << "SET ATTR FAILED" << endl;
		exit(-1);
	}

	cout << "Serial initialized successfully!" << endl;
}


// 读串口数据，并进行数据换算得到真实数据
void Serial::getData()
{
	ushort usTemp[11] = { 0 };
	short sTemp[22] = { 0 };

	/*读一组数据*/
	while (1)
	{
		if (read(serialFd, usTemp, 1) != 0)
		{
			cout << "READ FAILED" << endl;
			exit(-1);
		}

		/*一直读，直到"0x55 0x50"出现*/
		if (usTemp[0] != 0x55)
			continue;
		read(serialFd, &usTemp[1], 1);
		if (usTemp[1] != 0x50)
			continue;

		/*出现"0x55 0x50"后，再读31个字节，并进行和校验*/
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

		/*满足和校验，则写入*/
		memcpy(rawTime, &usTemp[5], 5);
		memcpy(rawAcc, &sTemp[2], 6);
		memcpy(rawAngle, &sTemp[13], 6);
		break;
	}

	/*进行数据处理，得到实际的时间、加速度和欧拉角*/
	// 时间
	time.hour = rawTime[0];
	time.minute = rawTime[1];
	time.second = rawTime[2];
	time.milisecond = (rawTime[4] << 8) | rawTime[3];

	short temp[3] = { 0 };
	// 加速度
	temp[0] = short(rawAcc[1] << 8 | rawAcc[0]);
	acc[0] = temp[0] / 32768 * 16 * 9.8;  // x轴
	temp[1] = short(rawAcc[3] << 8 | rawAcc[2]);
	acc[1] = temp[1] / 32768 * 16 * 9.8;  // y轴
	temp[2] = short(rawAcc[5] << 8 | rawAcc[4]);
	acc[2] = temp[3] / 32768 * 16 * 9.8;  // z轴
	accSum = abs(acc[0]) + abs(acc[1]) + abs(acc[2]);

	// 欧拉角
	temp[0] = short(rawAngle[1] << 8 | rawAngle[0]);
	angle[0] = temp[0] / 32768 * 180;  // x轴
	temp[1] = short(rawAngle[3] << 8 | rawAngle[2]);
	angle[1] = temp[1] / 32768 * 180;  // y轴
	temp[2] = short(rawAngle[5] << 8 | rawAngle[4]);
	angle[2] = temp[3] / 32768 * 180;  // z轴

}


// 关闭串口
void Serial::closePort()
{
	tcflush(serialFd, TCIFLUSH);  // 清空串口内容
	if (close(serialFd) != 0)  // 关闭串口
	{
		cout << "CLOSE FAILED" << endl;
		exit(-1);
	}
}


// 判断飞镖是否发射
bool Serial::isLaunched()
{
	if (accSum > 5 * 9.8)
		return true;
	else
		return false;
}


// 判断飞镖加速度是否稳定
bool Serial::isSteady()
{
	if ((accSum < 3 * 9.8) && (accSum > 0.8 * 9.8))
		return true;
	else
		return false;
}