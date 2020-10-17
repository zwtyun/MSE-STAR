#include "SerialPI.h"
#include "Control.h"
#include "Detect.h"

#define ulong unsigned long

using namespace std;
using namespace cv;

int main()
{
	/*������������*/
	Serial imu;
	imu.openPort();  // �򿪴���
	imu.initPort();  // ��ʼ������
	// ȷ���ѷ���
	while (!imu.isLaunched())
	{
		imu.getData();
	}
	// ȷ�ϼ��ٶ����ȶ�
	while (!imu.isSteady())
	{
		imu.getData();
	}
	// ��ʼ��ʱ����ʱ1s��
	imu.getData();
	ulong t0 = 3600 * 1000 * imu.time.hour + 60 * 1000 * imu.time.minute + 1000 * imu.time.second + imu.time.milisecond;
	while (1)
	{
		imu.getData();
		ulong t1 = 3600 * 1000 * imu.time.hour + 60 * 1000 * imu.time.minute + 1000 * imu.time.second + imu.time.milisecond;
		if ((t1 - t0) >= 1000)
			break;
	}


	/*��ͼ*/
	Armor armor;
	VideoCapture cap;  /**********�����Ҫ���㷨�Ż������������turbojpeg�Ż���ͼ�ٶ�***********/
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
		/*��ʼʶ��*/
		cap.read(frame);  
		armor.detect(frame);  //�������
		armor.match();  //�������
		armor.confirm(frame);  //ȷ�ϴ��Ŀ��
		armor.guide(frame);  //������һ���ķ��з���

		/*���Ϳ���ָ��*/
		motor.adjust(armor.dir);
	}

	return 0;
}