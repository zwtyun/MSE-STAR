#include "Detect.h"
#include <math.h>

using namespace cv;
using namespace std;

// detect()里用到的阈值
#define barMinHeight 6  //灯条最小高度
#define barMinWidth 2  //灯条最小宽度
#define barMinArea 12  //灯条最小面积
#define barMinRatio_length 4  //灯条最小高宽比
#define barMaxRatio_length 10  //灯条最大高宽比
#define barMinRatio_area 4/5  //灯条与其minRect的最小面积比

// match()里用到的阈值
#define barMaxAngleDiff 30  //两匹配灯条的最大角度差
#define barMaxDistCoeff 5  //两匹配灯条的最大中心距系数

// confirm()里用到的阈值
#define cirMinHeight 3  //引导灯最小高度
#define cirMinWidth 6  //引导灯最小宽度
#define cirMinArea 12  //引导灯最小面积
#define cirMinRatio_length 0.60  //引导灯最小高宽比
#define cirMinRatio_area 0.65  //引导灯与其minRect最小面积比
#define cirMaxRatio_area 0.90  //引导灯与其minRect最大面积比
#define cirMaxAngleDiff 30  //连线与灯条的最大角度差
#define cirMinRatio_dist 0.8  //连线长度与两灯条距离之比的最小值
#define cirMaxRatio_dist 1.1  //连线长度与两灯条距离之比的最大值
#define cirMaxRatio_area_bar 0.65  //灯条与引导灯的最大面积比


Armor::Armor()
{
	kernel = getStructuringElement(MORPH_ELLIPSE, Size(1, 2));
	confirmed_armor.center = Point(0, 0);
}


// 灯条检测
void Armor::detect(Mat src)  //输入BGR图像
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

	/*找灯条*/
	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect minRect;
		minRect = minAreaRect(contours[i]);
		// 调整矩形的宽高，使矩形的高大于宽，且角度增加90°
		if (minRect.size.width > minRect.size.height)
		{
			float temp;
			temp = minRect.size.width;
			minRect.size.width = minRect.size.height;
			minRect.size.height = temp;
			minRect.angle += 90;
		}

		// 高宽太小，面积太小，去掉
		if ((minRect.size.height < barMinHeight) || (minRect.size.width < barMinWidth) || (minRect.size.area < barMinArea))
			continue;
		// 高宽比不符要求，去掉
		float barRatio_length = minRect.size.height / minRect.size.width;
		if ((barRatio_length < barMinRatio_length) || (barRatio_length > barMaxRatio_length))
			continue;
		// 面积比太小，去掉
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

// 灯条配对
void Armor::match()
{
	//按横坐标排序
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
				(abs(lightBars[i].angle - lightBars[j].angle) > barMaxAngleDiff) ||  //两灯条角度相差过大
				((lightBars[j].center.x - lightBars[i].center.x) > (barMaxDistCoeff * ((lightBars[i].size.height > lightBars[j].size.height) ? lightBars[i].size.height : lightBars[j].size.height)))  //中心距离之比过大
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


// 确认打击目标
void Armor::confirm(Mat src)
{
	inRange(src, Scalar(160, 160, 160), Scalar(255, 255, 255), green);
	GaussianBlur(green, green, Size(5, 5), 0, 0);
	//imshow("green", green);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(green, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	/*找引导灯和最终要打击的装甲板*/
	for (int i = 0; i < contours.size(); i++)
	{
		RotatedRect minRect;
		minRect = minAreaRect(contours[i]);
		// 调整矩形的宽高，使矩形的宽大于高
		if (minRect.size.width < minRect.size.height)
		{
			float temp;
			temp = minRect.size.width;
			minRect.size.width = minRect.size.height;
			minRect.size.height = temp;
		}

		// 高宽太小，面积太小，去掉
		if ((minRect.size.height < cirMinHeight) || (minRect.size.width < cirMinWidth) || (minRect.size.area < cirMinArea))
			continue;
		// 高宽比太小，去掉（理论上应大于15/22 = 0.6818）
		float cirRatio_length = minRect.size.height / minRect.size.width;
		if (cirRatio_length < cirMinRatio_length)
			continue;
		// 面积比不符要求，去掉（理论上是pi/4 = 0.7854）
		float cirRatio_area = minRect.size.area / contourArea(contours[i]);
		if ((cirRatio_area < cirMinRatio_area) || cirRatio_area > cirMaxRatio_area)
			continue;

		/*与装甲板进行配对*/
		for (int j = 0; j < (int)armors.size(); j++)
		{
			double deltaX1 = armors[j].center.x - minRect.center.x;
			double deltaY1 = armors[j].center.y - minRect.center.y;
			double deltaX2 = armors[j].left_lightbar.center.x - armors[j].right_lightbar.center.x;
			double deltaY2 = armors[j].left_lightbar.center.y - armors[j].right_lightbar.center.y;
		
			double theta;  // 装甲板中心与引导灯中心连线，与x轴正半轴的夹角
			if (deltaX1 == 0)
				theta = 0;
			else
				theta = atan(deltaY1 / deltaX1);
			double distance1 = sqrt(pow(deltaX1, 2) + pow(deltaY1, 2));  // 装甲板中心与引导灯中心的连线的长度
			double distance2 = sqrt(pow(deltaX2, 2) + pow(deltaY2, 2));  // 装甲板左右灯条的距离
			double cirRatio_dist = distance1 / distance2;  // 连线长度与两灯条距离比
			double cirRatio_area_bar = (armors[j].left_lightbar.size.area + armors[j].right_lightbar.size.area) / contourArea(contours[i]);  // 灯条面积和与引导灯面积之比


			// 连线角度与两灯条角度相差过大，去掉
			if ((abs(theta - armors[j].left_lightbar.angle) > cirMaxAngleDiff) || (abs(theta - armors[j].right_lightbar.angle) > cirMaxAngleDiff))
				continue;
			// 连线长度与两灯条距离比不符要求，去掉（在装甲板的正视图中，距离比为39/43 = 0.9070）
			if ((cirRatio_dist < cirMinRatio_dist) || (cirRatio_dist > cirMaxRatio_dist))
				continue;
			// 面积比太大，去掉（在装甲板的正视图中，面积比为65/113 = 0.5752）
			if (cirRatio_area_bar > cirMaxRatio_area_bar)
				continue;

			// 确认最终要打击的装甲板
			confirmed_armor = armors[j];
			break;
		}

		if (confirmed_armor.center != Point(0, 0))
			break;
	}
}


// 给出下一步的飞行方向
void Armor::guide(Mat src)
{
	float segLine_y1 = 2 / 5 * src.rows;
	float segLine_y2 = 3 / 5 * src.rows;
	float segLine_x1 = 2 / 5 * src.cols;
	float segLine_x2 = 3 / 5 * src.cols;
	Point core = confirmed_armor.center;

	// 若不在视野里，就直着飞
	if ((core.x < 0) || (core.x > src.cols) || (core.y < 0) || (core.y > src.rows))
		dir = STRAIGHT;
	// 在左边
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
		// 在中间
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
		// 在右边
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