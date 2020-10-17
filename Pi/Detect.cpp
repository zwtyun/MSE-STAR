#include "Detect.h"
#include <math.h>

using namespace cv;
using namespace std;

// detect()���õ�����ֵ
#define barMinHeight 6  //������С�߶�
#define barMinWidth 2  //������С���
#define barMinArea 12  //������С���
#define barMinRatio_length 4  //������С�߿��
#define barMaxRatio_length 10  //�������߿��
#define barMinRatio_area 4/5  //��������minRect����С�����

// match()���õ�����ֵ
#define barMaxAngleDiff 30  //��ƥ����������ǶȲ�
#define barMaxDistCoeff 5  //��ƥ�������������ľ�ϵ��

// confirm()���õ�����ֵ
#define cirMinHeight 3  //��������С�߶�
#define cirMinWidth 6  //��������С���
#define cirMinArea 12  //��������С���
#define cirMinRatio_length 0.60  //��������С�߿��
#define cirMinRatio_area 0.65  //����������minRect��С�����
#define cirMaxRatio_area 0.90  //����������minRect��������
#define cirMaxAngleDiff 30  //��������������ǶȲ�
#define cirMinRatio_dist 0.8  //���߳���������������֮�ȵ���Сֵ
#define cirMaxRatio_dist 1.1  //���߳���������������֮�ȵ����ֵ
#define cirMaxRatio_area_bar 0.65  //�����������Ƶ���������


Armor::Armor()
{
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(1, 2));
	confirmed_armor.center = Point(0, 0);
}


// �������
void Armor::detect(Mat src)  //����BGRͼ��
{
	// Mat temp_mat;
	//	cvtColor(Input_Img, temp_mat, COLOR_BGR2HSV);
	//	inRange(temp_mat, Scalar(0, 0, 240), Scalar(255, 255, 255), mask);
	//	inRange(origin, Scalar(0, 0, 100), Scalar(100, 100, 255),mask);
	inRange(src, Scalar(0, 0, 170), Scalar(150, 150, 255), mask);

	imshow("mask", mask);
	morphologyEx(mask, mask, MORPH_OPEN, kernel);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarcy;
	findContours(mask, contours, hierarcy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	/*�ҵ���*/
	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect minRect;
		minRect = minAreaRect(contours[i]);
		// �������εĿ�ߣ�ʹ���εĸߴ��ڿ��ҽǶ�����90��
		if (minRect.size.width > minRect.size.height)
		{
			float temp;
			temp = minRect.size.width;
			minRect.size.width = minRect.size.height;
			minRect.size.height = temp;
			minRect.angle += 90;
		}

		// �߿�̫С�����̫С��ȥ��
		if ((minRect.size.height < barMinHeight) || (minRect.size.width < barMinWidth) || (minRect.size.area < barMinArea))
			continue;
		// �߿�Ȳ���Ҫ��ȥ��
		float barRatio_length = minRect.size.height / minRect.size.width;
		if ((barRatio_length < barMinRatio_length) || (barRatio_length > barMaxRatio_length))
			continue;
		// �����̫С��ȥ��
		float barRatio_area = minRect.size.area / contourArea(contours[i]);
		if (barRatio_area < barMinRatio_area)
			continue;

		lightBars.push_back(minRect);

		/*Point2f d[4];
		for (int i = 0; i < LightBars.size(); i++)
		{
			 LightBars[i].points(d);
			 line(src, d[0], d[1], Scalar(0, 255, 0));
			 line(src, d[2], d[1], Scalar(0, 255, 0));
			 line(src, d[2], d[3], Scalar(0, 255, 0));
			 line(src, d[0], d[3], Scalar(0, 255, 0));
		}
		imshow("lightBars",src);*/
	}
}

// �������
void Armor::match()
{
	//������������
	for (int i = 0; i < (int)lightBars.size() - 1; i++)
	{
		for (int j = i + 1; j < (int)lightBars.size(); j++)
		{
			if (lightBars[i].center.x > lightBars[j].center.x)
				swap(lightBars[i], lightBars[j]);
		}
	}
	/*Point2f d[4];
	for (int i = 0; i < Lightbars.size(); i++)
	{
		Lightbars[i].points(d);
		line(src, d[0], d[1], Scalar(0, 255, 0));
		line(src, d[2], d[1], Scalar(0, 255, 0));
		line(src, d[2], d[3], Scalar(0, 255, 0));
		line(src, d[0], d[3], Scalar(0, 255, 0));
	}
	imshow("lightBars", src);*/

	for (int i = 0; i < (int)lightBars.size() - 1; i++)
	{
		for (int j = i + 1; j < lightBars.size(); j++)
		{
			float t = ((lightBars[j].center.x - lightBars[i].center.x) / abs(lightBars[i].center.y - lightBars[j].center.y));

			if (
				(abs(lightBars[i].angle - lightBars[j].angle) > barMaxAngleDiff) ||  //�������Ƕ�������
				((lightBars[j].center.x - lightBars[i].center.x) > (barMaxDistCoeff * ((lightBars[i].size.height > lightBars[j].size.height) ? lightBars[i].size.height : lightBars[j].size.height)))  //���ľ���֮�ȹ���
				)
			{
				continue;
			}

			armor_data armor;
			armor.center.x = (lightBars[i].center.x + lightBars[j].center.x) / 2;
			armor.center.y = (lightBars[i].center.y + lightBars[j].center.y) / 2;
			armor.left_lightbar = lightBars[i];
			armor.right_lightbar = lightBars[j];
			armors.push_back(armor);
			lightBars.erase(lightBars.begin() + j);
			lightBars.erase(lightBars.begin() + i);
			i--;
			break;
		}
	}
}


// ȷ�ϴ��Ŀ��
void Armor::confirm(Mat src)
{
	inRange(src, Scalar(160, 160, 160), Scalar(255, 255, 255), green);
	GaussianBlur(green, green, Size(5, 5), 0, 0);
	//imshow("green", green);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(green, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	/*�������ƺ�����Ҫ�����װ�װ�*/
	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect minRect;
		minRect = minAreaRect(contours[i]);
		// �������εĿ�ߣ�ʹ���εĿ���ڸ�
		if (minRect.size.width < minRect.size.height)
		{
			float temp;
			temp = minRect.size.width;
			minRect.size.width = minRect.size.height;
			minRect.size.height = temp;
		}

		// �߿�̫С�����̫С��ȥ��
		if ((minRect.size.height < cirMinHeight) || (minRect.size.width < cirMinWidth) || (minRect.size.area < cirMinArea))
			continue;
		// �߿��̫С��ȥ����������Ӧ����15/22 = 0.6818��
		float cirRatio_length = minRect.size.height / minRect.size.width;
		if (cirRatio_length < cirMinRatio_length)
			continue;
		// ����Ȳ���Ҫ��ȥ������������pi/4 = 0.7854��
		float cirRatio_area = minRect.size.area / contourArea(contours[i]);
		if ((cirRatio_area < cirMinRatio_area) || cirRatio_area > cirMaxRatio_area)
			continue;

		/*��װ�װ�������*/
		for (int j = 0; j < (int)armors.size(); j++)
		{
			double deltaX1 = armors[j].center.x - minRect.center.x;
			double deltaY1 = armors[j].center.y - minRect.center.y;
			double deltaX2 = armors[j].left_lightbar.center.x - armors[j].right_lightbar.center.x;
			double deltaY2 = armors[j].left_lightbar.center.y - armors[j].right_lightbar.center.y;
		
			double theta;  // װ�װ��������������������ߣ���x��������ļн�
			if (deltaX1 == 0)
				theta = 0;
			else
				theta = atan(deltaY1 / deltaX1);
			double distance1 = sqrt(pow(deltaX1, 2) + pow(deltaY1, 2));  // װ�װ����������������ĵ����ߵĳ���
			double distance2 = sqrt(pow(deltaX2, 2) + pow(deltaY2, 2));  // װ�װ����ҵ����ľ���
			double cirRatio_dist = distance1 / distance2;  // ���߳����������������
			double cirRatio_area_bar = (armors[j].left_lightbar.size.area + armors[j].right_lightbar.size.area) / contourArea(contours[i]);  // ��������������������֮��


			// ���߽Ƕ����������Ƕ�������ȥ��
			if ((abs(theta - armors[j].left_lightbar.angle) > cirMaxAngleDiff) || (abs(theta - armors[j].right_lightbar.angle) > cirMaxAngleDiff))
				continue;
			// ���߳���������������Ȳ���Ҫ��ȥ������װ�װ������ͼ�У������Ϊ39/43 = 0.9070��
			if ((cirRatio_dist < cirMinRatio_dist) || (cirRatio_dist > cirMaxRatio_dist))
				continue;
			// �����̫��ȥ������װ�װ������ͼ�У������Ϊ65/113 = 0.5752��
			if (cirRatio_area_bar > cirMaxRatio_area_bar)
				continue;

			// ȷ������Ҫ�����װ�װ�
			confirmed_armor = armors[j];
			break;
		}

		if (confirmed_armor.center != Point(0, 0))
			break;
	}
}


// ������һ���ķ��з���
void Armor::guide(Mat src)
{
	float segLine_y1 = 2 / 5 * src.rows;
	float segLine_y2 = 3 / 5 * src.rows;
	float segLine_x1 = 2 / 5 * src.cols;
	float segLine_x2 = 3 / 5 * src.cols;
	Point core = confirmed_armor.center;

	// ��������Ұ���ֱ�ŷ�
	if ((core.x < 0) || (core.x > src.cols) || (core.y < 0) || (core.y > src.rows))
		dir = STRAIGHT;
	// �����
	if ((core.x >= 0) && (core.x < segLine_x1))
	{
		if ((core.y >= 0) && (core.y < segLine_y1))
			dir = LEFT_UP;
		else
		{
			if ((core.y >= segLine_y1) && (core.y <= segLine_y2))
				dir = LEFT;
			else
				dir = LEFT_DOWN;
		}
	}

	else
	{
		// ���м�
		if ((core.x >= segLine_x1) && (core.x <= segLine_x2))
		{
			if ((core.y >= 0) && (core.y < segLine_y1))
				dir = UP;
			else
			{
				if ((core.y >= segLine_y1) && (core.y <= segLine_y2))
					dir = STRAIGHT;
				else
					dir = DOWN;
			}
		}
		// ���ұ�
		else
		{
			if ((core.y >= 0) && (core.y < segLine_y1))
				dir = RIGHT_UP;
			else
			{
				if ((core.y >= segLine_y1) && (core.y <= segLine_y2))
					dir = RIGHT;
				else
					dir = RIGHT_DOWN;
			}
		}
	}
}