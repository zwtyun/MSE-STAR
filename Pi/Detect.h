#pragma once
#ifndef _DETECT_H_
#define _DETECT_H_
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

typedef struct {
	cv::Point center;  //���ĵ�
	cv::RotatedRect left_lightbar;
	cv::RotatedRect right_lightbar;
}armor_data;  //װ�װ���Ϣ

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
	std::vector<cv::RotatedRect> lightBars;  //��������
	std::vector<armor_data> armors;  //װ�װ�����
	armor_data confirmed_armor;  //���ȷ��Ҫ�����װ�װ�
	direction dir;  //��һ���ķ��з���

	Armor();
	void detect(cv::Mat src);  //�������
	void match();  //�������
	void confirm(cv::Mat src);  //ȷ�ϴ��Ŀ��
	void guide(cv::Mat src);  //������һ���ķ��з���
	
private:
	cv::Mat kernel;  //������̬ѧ�˲��ĺ�
	cv::Mat mask;  //������ֵͼ
	cv::Mat green;  //�����ƶ�ֵͼ
};


#endif // !_DETECT_H_
