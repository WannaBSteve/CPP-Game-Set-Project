#pragma once
#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
void drawAlpha(
    IMAGE* image, 			// ͼ��ָ��
    int x, int y, 			// �������
    int width, int height, 	 // ����ߴ�
    int pic_x, int pic_y, 	 // ͼ���е�λ��
    double AA = 1			// ͸����
);
//102,126
#define WindowWidth 480
#define WindowHeight 850