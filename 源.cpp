#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include"tools.h"
#pragma comment(lib,"Winmm.lib")

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
	int sheild;//护盾
	bool haveshield;//飞机是否还有护盾
	int rank;//用rank军衔来表示得分，当军衔为3的时候就通关了
	double score;//score为得分，每击落一个敌人得0.2分，得分大于军衔的时候说明该升级军衔了
	bool isDie;//飞机是否死亡
	bool undefeat;//飞机是否是无敌状态
	int crashtime;//最近一次发生撞击的时间
	int frame;//帧
	int hitframe;//给护盾做特殊动画的帧
	int rankframe;//晋升军衔的动画帧
	

	void setUndefeat();
	void delUndefeat();
}gamer;
Plane enemy[ENEMY_NUM];
struct Bullet {
	int x, y;
	bool isDie;
}bullet[BULLET_NUM];
IMAGE flushimg;		//缓冲区,防止闪屏
IMAGE img_gamer[2];//玩家飞机的图片
IMAGE img_gamerhit[2];//护盾耗尽时，玩家飞机被撞击之后的图片
IMAGE img_sheildhit[5];//还有护盾时，玩家飞机被撞击之后的图片
IMAGE img_bk;		//背景图，也是贴图的主界面
IMAGE img_bullet[2];
IMAGE img_raser[5];
IMAGE img_enemy[2];
IMAGE img_healthbar[7];
IMAGE img_hp;
IMAGE img_sheildbar[5];
IMAGE img_rank[6][12];




void drawRank(Plane* pthis, int Rankframe);
void drawHpbar(Plane* plane);
void drawsheildbar(Plane* plane);
//飞机初始化
void init_Plane(Plane* pthis, int x, int y) {
	pthis->isDie = false;
	pthis->x = x;
	pthis->y = y;
	pthis->frame = 0;
	pthis->hitframe = 0;
	pthis->HP = 6;
	pthis->sheild = 4;
	pthis->haveshield = true;
	pthis->undefeat = false;	//最开始不是无敌状态，只有扣了血的一段时间内或者捡到道具才进入
	pthis->rank = 0;
	pthis->score = -1;//!!!!!!!注意这里要从-1开始，才能让score大于军衔的时候晋升军衔
}
//绘制飞机
void draw_Plane(Plane* pthis) {
	if(pthis->undefeat==false)drawAlpha(img_gamer + pthis->frame, pthis->x, pthis->y, 102, 126, 0, 0, 1);
	else if (pthis->undefeat = true) {
		if (pthis->haveshield == false ) 
			drawAlpha(img_gamerhit + pthis->frame, pthis->x, pthis->y, 102, 126, 0, 0, 1);
		else if (pthis->haveshield == true && pthis->sheild == 0) 
		{
			pthis->haveshield = false;
			drawAlpha(img_sheildhit + pthis->hitframe, pthis->x, pthis->y, 102, 126, 0, 0, 1);
		}
		else drawAlpha(img_sheildhit + pthis->hitframe, pthis->x, pthis->y, 102, 126, 0, 0, 1);
	}
	int Rankframe = pthis->rankframe;
	drawRank(pthis,Rankframe);
	drawHpbar(pthis);
	drawsheildbar(pthis);
	pthis->rankframe = (pthis->rankframe + 1) % 12;
	pthis->hitframe = (pthis->hitframe + 1) % 5;
	pthis->frame = (pthis->frame + 1) % 2;

	LOGFONT f;
	gettextstyle(&f);						// 获取当前字体设置
	f.lfHeight = 48;						// 设置字体高度为 48
	_tcscpy(f.lfFaceName, _T("黑体"));		// 设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfQuality = ANTIALIASED_QUALITY;		// 设置输出效果为抗锯齿  
	TCHAR fontt[] = "黑体";
	TCHAR score[] = "Score ";
	char s[2];
	settextstyle(18, 10, fontt);
	outtextxy(365, 100, score);
	sprintf(s, "%d", pthis->rank);
	outtextxy(420, 100, s);
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
		if (pthis->score > 5) { pthis->isDie = true; break; }
		if (pthis->score >= pthis->rank)pthis->rank++;
		if (pthis->HP == 0) {
			pthis->isDie = true;		
			break;
		}//如果生命值为零游戏结束
		if (pthis->sheild == 0) { //当护盾为0之后开始扣血
			
			if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
		
		}
		else {
			if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//记录最近一次撞击时间
				pthis->setUndefeat();//设置为短暂无敌
			}
			
		}
		if (clock() - pthis->crashtime >= 2000)pthis->delUndefeat();		//无敌时间为2s
		
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
void moveEnemy(Plane*pthis) {
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
						//还要加得分用来升级rank
						pthis->score += 0.2;
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
	
	loadimage(&img_bk, "Nebula.png", 480, 850); 
	loadimage(img_gamer + 0, "me1.png");
	loadimage(img_gamer + 1, "me2.png");
	loadimage(img_gamerhit + 0, "me1_hit.png");
	loadimage(img_gamerhit + 1, "me2_hit.png");
	loadimage(img_sheildhit + 0, "me2_sheild.png");
	loadimage(img_sheildhit + 1, "me2_sheild1.png");
	loadimage(img_sheildhit + 2, "me2_sheild2.png");
	loadimage(img_sheildhit + 3, "me2_sheild3.png");
	loadimage(img_sheildhit + 4, "me2_sheild4.png");
	//加载子弹
	loadimage(img_bullet + 0, "bullet1.png");
	loadimage(img_bullet + 1, "bullet2.png");
	loadimage(img_raser+0, "raserbullet0.png");
	loadimage(img_raser + 1, "raserbullet1.png");
	loadimage(img_raser + 2, "raserbullet2.png");
	loadimage(img_raser + 3, "raserbullet3.png");
	loadimage(img_raser + 4, "raserbullet4.png");
	//敌机
	loadimage(img_enemy + 0, "enemy1.png");
	loadimage(img_enemy + 1, "enemy2.png");
	//生命值和基本属性
	loadimage(img_healthbar+0,"healthbar.png");
	loadimage(img_healthbar+1, "healthbar-1.png");
	loadimage(img_healthbar+2, "healthbar-2.png");
	loadimage(img_healthbar+3, "healthbar-3.png");
	loadimage(img_sheildbar+0, "sheildbar.png");
	loadimage(img_sheildbar + 1, "sheildbar-1.png");
	loadimage(img_sheildbar + 2, "sheildbar-2.png");
	loadimage(img_sheildbar + 3, "sheildbar-3.png");
	loadimage(img_sheildbar + 4, "sheildbar-4.png");

	loadimage(img_rank[0] + 0, "1@64px.png");
	loadimage(img_rank[0] + 1, "1@64px.png");
	loadimage(img_rank[0] + 2, "2@64px.png");
	loadimage(img_rank[0] + 3, "2@64px.png");
	loadimage(img_rank[0] + 4, "3@64px.png");
	loadimage(img_rank[0] + 5, "3@64px.png");
	loadimage(img_rank[0] + 6, "4@64px.png");
	loadimage(img_rank[0] + 7, "4@64px.png");
	loadimage(img_rank[0] + 8, "5@64px.png");
	loadimage(img_rank[0] + 9, "5@64px.png");
	loadimage(img_rank[0] + 10, "6@64px.png");
	loadimage(img_rank[0] + 11, "6@64px.png");

	loadimage(img_rank[1] + 0, "1.1.png");
	loadimage(img_rank[1] + 1, "1.1.png");
	loadimage(img_rank[1] + 2, "1.2.png");
	loadimage(img_rank[1] + 3, "1.2.png");
	loadimage(img_rank[1] + 4, "1.3.png");
	loadimage(img_rank[1] + 5, "1.3.png");
	loadimage(img_rank[1] + 6, "1.4.png");
	loadimage(img_rank[1] + 7, "1.4.png");
	loadimage(img_rank[1] + 8, "1.5.png");
	loadimage(img_rank[1] + 9, "1.5.png");
	loadimage(img_rank[1] + 10, "1.6.png");
	loadimage(img_rank[1] + 11, "1.6.png");

	loadimage(img_rank[2] + 0, "2.1.png");
	loadimage(img_rank[2] + 1, "2.1.png");
	loadimage(img_rank[2] + 2, "2.2.png");
	loadimage(img_rank[2] + 3, "2.2.png");
	loadimage(img_rank[2] + 4, "2.3.png");
	loadimage(img_rank[2] + 5, "2.3.png");
	loadimage(img_rank[2] + 6, "2.4.png");
	loadimage(img_rank[2] + 7, "2.4.png");
	loadimage(img_rank[2] + 8, "2.5.png");
	loadimage(img_rank[2] + 9, "2.5.png");
	loadimage(img_rank[2] + 10, "2.6.png");
	loadimage(img_rank[2] + 11, "2.6.png"); 

	loadimage(img_rank[3] + 0, "3.1.png");
	loadimage(img_rank[3] + 1, "3.1.png");
	loadimage(img_rank[3] + 2, "3.2.png");
	loadimage(img_rank[3] + 3, "3.2.png");
	loadimage(img_rank[3] + 4, "3.3.png");
	loadimage(img_rank[3] + 5, "3.3.png");
	loadimage(img_rank[3] + 6, "3.4.png");
	loadimage(img_rank[3] + 7, "3.4.png");
	loadimage(img_rank[3] + 8, "3.5.png");
	loadimage(img_rank[3] + 9, "3.5.png");
	loadimage(img_rank[3] + 10, "3.6.png");
	loadimage(img_rank[3] + 11, "3.6.png");

	loadimage(img_rank[4] + 0, "4.1.png");
	loadimage(img_rank[4] + 1, "4.1.png");
	loadimage(img_rank[4] + 2, "4.2.png");
	loadimage(img_rank[4] + 3, "4.2.png");
	loadimage(img_rank[4] + 4, "4.3.png");
	loadimage(img_rank[4] + 5, "4.3.png");
	loadimage(img_rank[4] + 6, "4.4.png");
	loadimage(img_rank[4] + 7, "4.4.png");
	loadimage(img_rank[4] + 8, "4.5.png");
	loadimage(img_rank[4] + 9, "4.5.png");
	loadimage(img_rank[4] + 10, "4.6.png");
	loadimage(img_rank[4] + 11, "4.6.png");

	loadimage(img_rank[5] + 0, "5.1.png");
	loadimage(img_rank[5] + 1, "5.1.png");
	loadimage(img_rank[5] + 2, "5.2.png");
	loadimage(img_rank[5] + 3, "5.2.png");
	loadimage(img_rank[5] + 4, "5.3.png");
	loadimage(img_rank[5] + 5, "5.3.png");
	loadimage(img_rank[5] + 6, "5.4.png");
	loadimage(img_rank[5] + 7, "5.4.png");
	loadimage(img_rank[5] + 8, "5.5.png");
	loadimage(img_rank[5] + 9, "5.5.png");
	loadimage(img_rank[5] + 10, "5.6.png");
	loadimage(img_rank[5] + 11, "5.6.png");

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
void drawRank(Plane* plane,int Rankframe) {
	int i = plane->rank-1;
	if(i>=0&&i<=4)
	drawAlpha(img_rank[i] + Rankframe, 385, 10, 128, 128, 0, 0, 1);
}
void drawHpbar(Plane* plane) {
	int i = 6 - plane->HP;
	drawAlpha(img_healthbar+i, 0, 5, 150, 34, 0, 0, 1);
}
void drawsheildbar(Plane* plane) {
	int i = 4 - plane->sheild;
	drawAlpha(img_sheildbar + i, 0, 39, 117, 27, 0, 0, 1);
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
			int j = gamer.score;
			drawAlpha(img_raser+j, bullet[i].x, bullet[i].y, 80, 80, 0, 0, 1);
			drawAlpha(img_raser+j, gamer.x, bullet[i].y, 80, 80, 0, 0, 1);
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

void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	if (dstimg == NULL)
	{
		return;
	}
	// 变量初始化
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = dstimg->getwidth();
	int dst_height = dstimg->getheight();

	// 实现透明贴图  可优化
	for (int iy = 0; iy < src_height; iy++)
	{
		for (int ix = 0; ix < src_width; ix++)
		{
			int srcX = ix + iy * src_width;
			int sa = ((src[srcX] & 0xff000000) >> 24);
			int sr = ((src[srcX] & 0xff0000) >> 16);
			int sg = ((src[srcX] & 0xff00) >> 8);
			int sb = src[srcX] & 0xff;
			if (x + ix >= 0 && x + ix < dst_width
				&& y + iy >= 0 && y + iy < dst_height)
			{
				int dstX = (x + ix) + (y + iy) * dst_width;
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
					| (sb * sa / 255 + db * (255 - sa) / 255);
			}
		}
	}
}
void Play() {
	//创建窗口
	initgraph(480, 850);
L1: loadimage(&img_bk, "Nebula.png", 480, 850);
	loadimage(img_healthbar + 0, "healthbar.png");
	loadimage(img_healthbar + 1, "healthbar-1.png");
	loadimage(img_healthbar + 2, "healthbar-2.png");
	loadimage(img_healthbar + 3, "healthbar-3.png");
	loadimage(img_healthbar + 4, "healthbar-4.png");
	loadimage(img_healthbar + 5, "healthbar-5.png");
	loadimage(img_healthbar + 6, "healthbar-6.png");

	loadimage(img_sheildbar + 0, "sheildbar.png");
	loadimage(img_sheildbar + 1, "sheildbar-1.png");
	loadimage(img_sheildbar + 2, "sheildbar-2.png");
	loadimage(img_sheildbar + 3, "sheildbar-3.png");
	loadimage(img_sheildbar + 4, "sheildbar-4.png");
	putimage(0, 0, &img_bk);


	TCHAR s[50] = "1.开始游戏";
	TCHAR s1[50] = "2.游戏帮助";
	TCHAR s2[50] = "3.退出";
	button(155, 210, 170, 50, s);
	button(155, 360, 170, 50, s1);
	button(155, 510, 170, 50, s2);
	MOUSEMSG m;
	while (true) {
		//播放音乐
		mciSendString("open cornfield_chase.mp3 alias mymusic", NULL, 0, NULL);
		mciSendString("play mymusic", NULL, 0, NULL);
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
						HWND wnd = GetHWnd();
						if (MessageBox(wnd, "您是否想要重来一局？", "飞机已阵亡！", MB_YESNO) == 6)Play();
						else
							return;
					}
					moveBullet();
					//隔一段时间产生一个敌机
					if (Timer(200, 1)) {
						createEnemy();
					}
					moveEnemy(&gamer);
					int frameTime = clock() - startTime;
					//一帧应该执行的时间大于当前帧执行的时间（提前完成了）
					if (1000 / 60 - frameTime > 0) {
						Sleep(1000 / 60 - frameTime);
					}

					//双缓冲消除闪屏
					drawAlpha(&flushimg, 0, 0, &img_bk);
					drawAlpha(&img_bk, 0, 0, &flushimg);
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
				return ;
			}
		}
	}
	system("pause");
	return ;
}
int main() {
	Play();
	return 0;
}

void Plane::setUndefeat()
{
	undefeat = true;
}

void Plane::delUndefeat()
{
	undefeat = false;
}
