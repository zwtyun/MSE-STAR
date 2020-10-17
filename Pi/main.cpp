#include "SerialPI.h"
#include "Control.h"
#include "Detect.h"

#define ulong unsigned long

using namespace std;
using namespace cv;

int main()
{
	/*读陀螺仪数据*/
	Serial imu;
	imu.openPort();  // 打开串口
	imu.initPort();  // 初始化串口
	// 确认已发射
	while (!imu.isLaunched())
	{
		imu.getData();
	}
	// 确认加速度已稳定
	while (!imu.isSteady())
	{
		imu.getData();
	}
	// 开始计时（定时1s）
	imu.getData();
	ulong t0 = 3600 * 1000 * imu.time.hour + 60 * 1000 * imu.time.minute + 1000 * imu.time.second + imu.time.milisecond;
	while (1)
	{
		imu.getData();
		ulong t1 = 3600 * 1000 * imu.time.hour + 60 * 1000 * imu.time.minute + 1000 * imu.time.second + imu.time.milisecond;
		if ((t1 - t0) >= 1000)
			break;
	}


	/*读图*/
	Armor armor;
	VideoCapture cap;  /**********如果需要做算法优化，这里可以用turbojpeg优化读图速度***********/
	Mat frame;
	cap.open(0);
	if (!cap.isOpened())
	{
		cout << "CAM OPEN FAILED" << endl;
		exit(1);
	}
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);

	Motor motor;
	while (1)
	{
		/*开始识别*/
		cap.read(frame);  
		armor.detect(frame);  //灯条检测
		armor.match();  //灯条配对
		armor.confirm(frame);  //确认打击目标
		armor.guide(frame);  //给出下一步的飞行方向

		/*发送控制指令*/
		motor.adjust(armor.dir);
	}

	return 0;
}