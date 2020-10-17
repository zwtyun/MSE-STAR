#pragma once
#ifndef _DETECT_H_
#define _DETECT_H_
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

typedef struct {
	cv::Point center;  //中心点
	cv::RotatedRect left_lightbar;
	cv::RotatedRect right_lightbar;
}armor_data;  //装甲板信息

enum direction{
	STRAIGHT,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	LEFT_UP,
	RIGHT_UP,
	LEFT_DOWN,
	RIGHT_DOWN
};

class Armor
{
public:
	std::vector<cv::RotatedRect> lightBars;  //灯条容器
	std::vector<armor_data> armors;  //装甲板容器
	armor_data confirmed_armor;  //最后确认要击打的装甲板
	direction dir;  //下一步的飞行方向

	Armor();
	void detect(cv::Mat src);  //灯条检测
	void match();  //灯条配对
	void confirm(cv::Mat src);  //确认打击目标
	void guide(cv::Mat src);  //给出下一步的飞行方向
	
private:
	cv::Mat kernel;  //用来形态学滤波的核
	cv::Mat mask;  //灯条二值图
	cv::Mat green;  //引导灯二值图
};


#endif // !_DETECT_H_
