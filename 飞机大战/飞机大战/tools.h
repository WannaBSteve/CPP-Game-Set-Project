#pragma once
#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
void drawAlpha(
    IMAGE* image, 			// 图像指针
    int x, int y, 			// 输出坐标
    int width, int height, 	 // 输出尺寸
    int pic_x, int pic_y, 	 // 图像中的位置
    double AA = 1			// 透明度
);
//102,126
#define WindowWidth 480
#define WindowHeight 850