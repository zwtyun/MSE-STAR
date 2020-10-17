#pragma once
#ifndef _SERIALPI_H_
#define _SERIALPI_H_

#define ushort unsigned short
using namespace std;

struct Time
{
	ushort hour;
	ushort minute;
	ushort second;
	ushort milisecond;
};

class Serial
{
public:
	int serialFd;
	struct Time time;
	float acc[3] = { 0 };
	float accSum = 0;
	float angle[3] = { 0 };

	void openPort();
	void initPort();
	void getData();
	void closePort();

	bool isLaunched();
	bool isSteady();

private:
	ushort rawTime[5] = { 0 };
	short rawAcc[6] = { 0 };
	short rawAngle[6] = { 0 };
};

#endif // !_SERIALPI_H_
