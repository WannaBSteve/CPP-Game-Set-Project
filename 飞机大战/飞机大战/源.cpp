#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include"tools.h"

#define BULLET_NUM 15
#define ENEMY_NUM 10

void createBullet();

//自定义计时器
bool Timer(int ms, int id) {
	static int start[5];
	int end = clock();
	if (end - start[id] >= ms) {
		start[id] = end;
		return true;
	}
	return false;
}

struct Plane {
	int x, y;
	int HP;//生命值
	bool isDie;//飞机是否死亡
	int frame;//帧
}gamer;
Plane enemy[ENEMY_NUM];
struct Bullet {
	int x, y;
	bool isDie;
}bullet[BULLET_NUM];
IMAGE img_gamer[2];//玩家飞机的图片
IMAGE img_bk;
IMAGE img_bullet[2];
IMAGE img_enemy[2];
IMAGE img_healthbar[7];
IMAGE img_hp;
IMAGE img_sheildbar;

void drawHpbar(Plane* plane);
//飞机初始化
void init_Plane(Plane* pthis, int x, int y) {
	pthis->isDie = false;
	pthis->x = x;
	pthis->y = y;
	pthis->frame = 0;
	pthis->HP = 6;
}
//绘制飞机
void draw_Plane(Plane* pthis) {
	drawAlpha(img_gamer + pthis->frame, pthis->x, pthis->y, 102, 126, 0, 0, 1);
	drawHpbar(pthis);
	drawAlpha(&img_sheildbar, 0, 37, 117, 27, 0, 0, 1);
	pthis->frame = (pthis->frame + 1) % 2;
}
//移动飞机
void move_Plane(Plane* pthis) {
	//获取键盘按键
	int speed = 15;
	if (GetAsyncKeyState(VK_UP)&&pthis->y>0) {
		pthis->y -= speed;
	}
	if (GetAsyncKeyState(VK_DOWN)&&pthis->y+img_gamer->getheight()<getheight()) {
		pthis->y += speed;
	}
	if (GetAsyncKeyState(VK_LEFT)&&pthis->x+img_gamer->getwidth()/2>0) {
		pthis->x -= speed;
	}
	if (GetAsyncKeyState(VK_RIGHT)&&pthis->x+img_gamer->getwidth()/2<getwidth()) {
		pthis->x += speed;
	}
	if (GetAsyncKeyState(VK_SPACE)&&Timer(200,0)) {
		createBullet();
	}
	//判断是否撞到敌机而死
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (pthis->HP == 0) {
			pthis->isDie = true;
			break;
		}//如果生命值为零游戏结束
		if (pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
			pthis->HP--;
		}
		if (pthis->x+img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer ->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
			pthis->HP--;
		}
		if (pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y+img_gamer->getheight() >= enemy[i].y && pthis->y+img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
			pthis->HP--;
		}
		if (pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
			pthis->HP--;
		}
	}
}
//创建敌机
void createEnemy() {
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (enemy[i].isDie) {
			//随机坐标
			enemy[i].x = rand() % getwidth();
			enemy[i].y = -img_enemy->getheight();
			enemy[i].isDie = false;
			break;
		}
	}
}
//移动敌机
void moveEnemy() {
	int speed = 3;
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (!enemy[i].isDie) {
			enemy[i].y += speed;
			if (enemy[i].y > getheight()) {
				enemy[i].isDie = true;
			}
			for (int j = 0; j < BULLET_NUM; ++j) {
				if (!bullet[j].isDie) {
					if (bullet[j].x >= enemy[i].x && bullet[j].x <= enemy[i].x + img_enemy->getwidth() && bullet[j].y >= enemy[i].y && bullet[j].y <= enemy[i].y + img_enemy->getheight()) {
						enemy[i].isDie = true;
						bullet[j].isDie = true;
					}
				}
			}
		}
	}
}
//移动子弹
void moveBullet() {
	int speed = 30;
	for (int i = 0; i < BULLET_NUM; ++i) {
		if (!bullet[i].isDie) {
			bullet[i].y -= speed;
			if (bullet[i].y < 0) {
				bullet[i].isDie = true;
			}
		}
	}
}
//加载图片
void loadResource() {
	//背景
	
	loadimage(&img_bk, "background.png", 480, 850);
	loadimage(img_gamer + 0, "me1.png");
	loadimage(img_gamer + 1, "me2.png");
	//加载子弹
	loadimage(img_bullet + 0, "bullet1.png");
	loadimage(img_bullet + 1, "bullet2.png");
	//敌机
	loadimage(img_enemy + 0, "enemy1.png");
	loadimage(img_enemy + 1, "enemy2.png");
	//生命值和基本属性
	loadimage(img_healthbar+0,"healthbar.png");
	loadimage(img_healthbar+1, "healthbar-1.png");
	loadimage(img_healthbar+2, "healthbar-2.png");
	loadimage(img_healthbar+3, "healthbar-3.png");
	loadimage(&img_sheildbar, "sheildbar.png");
	
	//加载help等button
	

}
//创建子弹
void createBullet() {
	for (int i = 0; i < BULLET_NUM; ++i) {
		if (bullet[i].isDie) {
			bullet[i].x = gamer.x + img_gamer->getwidth() / 2;
			bullet[i].y = gamer.y;
			bullet[i].isDie = false; 
			break;
		}
	}
}
//所有数据初始化
void init() {
	loadResource();
	//初始化玩家飞机
	init_Plane(&gamer, (getwidth() - img_gamer->getwidth()) / 2, getheight() - img_gamer->getheight());
	//初始化子弹
	for (int i = 0; i < BULLET_NUM; ++i) {
		bullet[i].isDie = true;
	}
	//初始化敌机
	for (int i = 0; i < ENEMY_NUM; ++i) {
		enemy[i].isDie = true;
	}
}
void drawHpbar(Plane* plane) {
	int i = 6 - plane->HP;
	drawAlpha(img_healthbar+i, 0, 5, 150, 34, 0, 0, 1);
}
//绘制界面
void draw() {
	//背景
	putimage(0, 0, &img_bk);
	//玩家
	draw_Plane(&gamer);
	
	//子弹
	for (int i = 0; i < BULLET_NUM; ++i) {
		if (!bullet[i].isDie) {
			drawAlpha(img_bullet + 0, bullet[i].x, bullet[i].y, 5, 11, 0, 0, 1);
		}
	}
	//敌机
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (!enemy[i].isDie) {
			drawAlpha(img_enemy + 0, enemy[i].x, enemy[i].y, 57, 43, 0, 0, 1);
		}
	}
}
//绘制按钮
void button(int x, int y, int w, int h, TCHAR* text)
{
	setbkmode(TRANSPARENT);
	setfillcolor(GREEN);
	fillroundrect(x, y, x + w, y + h, 10, 10);
	// 输出字符串（MBCS 字符集）
	TCHAR s1[] = "黑体";
	settextstyle(30, 0, s1);
	TCHAR s[50] = "hello";

	int tx = x + (w - textwidth(text)) / 2;
	int ty = y + (h - textheight(text)) / 2;

	outtextxy(tx, ty, text);

}
// 透明贴图（绘图函数），补充透明度 AA
void drawAlpha(IMAGE* image, int x, int y, int width, int height, int pic_x, int pic_y, double AA)
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();			// GetImageBuffer() 函数，用于获取绘图设备的显存指针， EasyX 自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(image);		// 获取 picture 的显存指针
	int imageWidth = image->getwidth();		// 获取图片宽度
	int imageHeight = image->getheight();	// 获取图片宽度
	int dstX = 0;							// 在 绘图区域 显存里像素的角标
	int srcX = 0;							// 在 image 显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			// 防止越界
			if (ix + pic_x >= 0 && ix + pic_x < imageWidth && iy + pic_y >= 0 && iy + pic_y < imageHeight &&
				ix + x >= 0 && ix + x < WindowWidth && iy + y >= 0 && iy + y < WindowHeight)
			{
				// 获取像素角标
				int srcX = (ix + pic_x) + (iy + pic_y) * imageWidth;
				dstX = (ix + x) + (iy + y) * WindowWidth;

				int sa = ((src[srcX] & 0xff000000) >> 24) * AA;			// 0xAArrggbb; AA 是透明度
				int sr = ((src[srcX] & 0xff0000) >> 16);				// 获取 RGB 里的 R
				int sg = ((src[srcX] & 0xff00) >> 8);					// G
				int sb = src[srcX] & 0xff;								// B

				// 设置对应的绘图区域像素信息
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
int main() {
	//创建窗口
	initgraph(480, 850);
L1: loadimage(&img_bk, "background.png",480,850);
	loadimage(img_healthbar + 0, "healthbar.png");
	loadimage(img_healthbar+1, "healthbar-1.png");
	loadimage(img_healthbar + 2, "healthbar-2.png");
	loadimage(img_healthbar + 3, "healthbar-3.png");
	loadimage(img_healthbar + 4, "healthbar-4.png");
	loadimage(img_healthbar + 5, "healthbar-5.png");
	loadimage(img_healthbar + 6, "healthbar-6.png");
	
	loadimage(&img_sheildbar, "sheildbar.png");
	putimage(0, 0, &img_bk);

	TCHAR s[50] = "1.开始游戏";
	TCHAR s1[50] = "2.游戏帮助";
	TCHAR s2[50] = "3.退出";
	button(155, 210, 170, 50, s);
	button(155, 360, 170, 50, s1);
	button(155, 510, 170, 50, s2);
	MOUSEMSG m;
	while (true) {

		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			if (m.x >= 155 && m.x <= 325 && m.y >= 210 && m.y <= 260) {
				//初始化
				init();
				while (true) {
					int startTime = clock();
					draw();
					move_Plane(&gamer);
					if (gamer.isDie) {
						return 0;
					}
					moveBullet();
					//隔一段时间产生一个敌机
					if (Timer(200, 1)) {
						createEnemy();
					}
					moveEnemy();
					int frameTime = clock() - startTime;
					//一帧应该执行的时间大于当前帧执行的时间（提前完成了）
					if (1000 / 60 - frameTime > 0) {
						Sleep(1000 / 60 - frameTime);
					}
				}
				system("pause");
			}
			else if (m.x >= 155 && m.x <= 325 && m.y >= 360 && m.y <= 410) {
				putimage(0, 0, &img_bk);
				//这一句目前没用  loadimage(&img_hp, "游戏帮助.png", 434, 131);
				drawAlpha(&img_hp, 30, 360, 434, 131, 0, 0, 1);
				Sleep(3000);
				goto L1;
			}
			else if (m.x >= 155 && m.x <= 325 && m.y >= 510 && m.y <= 560) {
				return 0;
			}
		}
	}
	system("pause");
	return 0;
}
