#include<stdio.h>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#include"tools.h"

#define BULLET_NUM 15
#define ENEMY_NUM 10

void createBullet();

//�Զ����ʱ��
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
	int HP;//����ֵ
	bool isDie;//�ɻ��Ƿ�����
	int frame;//֡
}gamer;
Plane enemy[ENEMY_NUM];
struct Bullet {
	int x, y;
	bool isDie;
}bullet[BULLET_NUM];
IMAGE img_gamer[2];//��ҷɻ���ͼƬ
IMAGE img_bk;
IMAGE img_bullet[2];
IMAGE img_enemy[2];
IMAGE img_healthbar[7];
IMAGE img_hp;
IMAGE img_sheildbar;

void drawHpbar(Plane* plane);
//�ɻ���ʼ��
void init_Plane(Plane* pthis, int x, int y) {
	pthis->isDie = false;
	pthis->x = x;
	pthis->y = y;
	pthis->frame = 0;
	pthis->HP = 6;
}
//���Ʒɻ�
void draw_Plane(Plane* pthis) {
	drawAlpha(img_gamer + pthis->frame, pthis->x, pthis->y, 102, 126, 0, 0, 1);
	drawHpbar(pthis);
	drawAlpha(&img_sheildbar, 0, 37, 117, 27, 0, 0, 1);
	pthis->frame = (pthis->frame + 1) % 2;
}
//�ƶ��ɻ�
void move_Plane(Plane* pthis) {
	//��ȡ���̰���
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
	//�ж��Ƿ�ײ���л�����
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (pthis->HP == 0) {
			pthis->isDie = true;
			break;
		}//�������ֵΪ����Ϸ����
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
//�����л�
void createEnemy() {
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (enemy[i].isDie) {
			//�������
			enemy[i].x = rand() % getwidth();
			enemy[i].y = -img_enemy->getheight();
			enemy[i].isDie = false;
			break;
		}
	}
}
//�ƶ��л�
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
//�ƶ��ӵ�
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
//����ͼƬ
void loadResource() {
	//����
	
	loadimage(&img_bk, "background.png", 480, 850);
	loadimage(img_gamer + 0, "me1.png");
	loadimage(img_gamer + 1, "me2.png");
	//�����ӵ�
	loadimage(img_bullet + 0, "bullet1.png");
	loadimage(img_bullet + 1, "bullet2.png");
	//�л�
	loadimage(img_enemy + 0, "enemy1.png");
	loadimage(img_enemy + 1, "enemy2.png");
	//����ֵ�ͻ�������
	loadimage(img_healthbar+0,"healthbar.png");
	loadimage(img_healthbar+1, "healthbar-1.png");
	loadimage(img_healthbar+2, "healthbar-2.png");
	loadimage(img_healthbar+3, "healthbar-3.png");
	loadimage(&img_sheildbar, "sheildbar.png");
	
	//����help��button
	

}
//�����ӵ�
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
//�������ݳ�ʼ��
void init() {
	loadResource();
	//��ʼ����ҷɻ�
	init_Plane(&gamer, (getwidth() - img_gamer->getwidth()) / 2, getheight() - img_gamer->getheight());
	//��ʼ���ӵ�
	for (int i = 0; i < BULLET_NUM; ++i) {
		bullet[i].isDie = true;
	}
	//��ʼ���л�
	for (int i = 0; i < ENEMY_NUM; ++i) {
		enemy[i].isDie = true;
	}
}
void drawHpbar(Plane* plane) {
	int i = 6 - plane->HP;
	drawAlpha(img_healthbar+i, 0, 5, 150, 34, 0, 0, 1);
}
//���ƽ���
void draw() {
	//����
	putimage(0, 0, &img_bk);
	//���
	draw_Plane(&gamer);
	
	//�ӵ�
	for (int i = 0; i < BULLET_NUM; ++i) {
		if (!bullet[i].isDie) {
			drawAlpha(img_bullet + 0, bullet[i].x, bullet[i].y, 5, 11, 0, 0, 1);
		}
	}
	//�л�
	for (int i = 0; i < ENEMY_NUM; ++i) {
		if (!enemy[i].isDie) {
			drawAlpha(img_enemy + 0, enemy[i].x, enemy[i].y, 57, 43, 0, 0, 1);
		}
	}
}
//���ư�ť
void button(int x, int y, int w, int h, TCHAR* text)
{
	setbkmode(TRANSPARENT);
	setfillcolor(GREEN);
	fillroundrect(x, y, x + w, y + h, 10, 10);
	// ����ַ�����MBCS �ַ�����
	TCHAR s1[] = "����";
	settextstyle(30, 0, s1);
	TCHAR s[50] = "hello";

	int tx = x + (w - textwidth(text)) / 2;
	int ty = y + (h - textheight(text)) / 2;

	outtextxy(tx, ty, text);

}
// ͸����ͼ����ͼ������������͸���� AA
void drawAlpha(IMAGE* image, int x, int y, int width, int height, int pic_x, int pic_y, double AA)
{
	// ������ʼ��
	DWORD* dst = GetImageBuffer();			// GetImageBuffer() ���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬 EasyX �Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(image);		// ��ȡ picture ���Դ�ָ��
	int imageWidth = image->getwidth();		// ��ȡͼƬ���
	int imageHeight = image->getheight();	// ��ȡͼƬ���
	int dstX = 0;							// �� ��ͼ���� �Դ������صĽǱ�
	int srcX = 0;							// �� image �Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			// ��ֹԽ��
			if (ix + pic_x >= 0 && ix + pic_x < imageWidth && iy + pic_y >= 0 && iy + pic_y < imageHeight &&
				ix + x >= 0 && ix + x < WindowWidth && iy + y >= 0 && iy + y < WindowHeight)
			{
				// ��ȡ���ؽǱ�
				int srcX = (ix + pic_x) + (iy + pic_y) * imageWidth;
				dstX = (ix + x) + (iy + y) * WindowWidth;

				int sa = ((src[srcX] & 0xff000000) >> 24) * AA;			// 0xAArrggbb; AA ��͸����
				int sr = ((src[srcX] & 0xff0000) >> 16);				// ��ȡ RGB ��� R
				int sg = ((src[srcX] & 0xff00) >> 8);					// G
				int sb = src[srcX] & 0xff;								// B

				// ���ö�Ӧ�Ļ�ͼ����������Ϣ
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}
int main() {
	//��������
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

	TCHAR s[50] = "1.��ʼ��Ϸ";
	TCHAR s1[50] = "2.��Ϸ����";
	TCHAR s2[50] = "3.�˳�";
	button(155, 210, 170, 50, s);
	button(155, 360, 170, 50, s1);
	button(155, 510, 170, 50, s2);
	MOUSEMSG m;
	while (true) {

		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			if (m.x >= 155 && m.x <= 325 && m.y >= 210 && m.y <= 260) {
				//��ʼ��
				init();
				while (true) {
					int startTime = clock();
					draw();
					move_Plane(&gamer);
					if (gamer.isDie) {
						return 0;
					}
					moveBullet();
					//��һ��ʱ�����һ���л�
					if (Timer(200, 1)) {
						createEnemy();
					}
					moveEnemy();
					int frameTime = clock() - startTime;
					//һ֡Ӧ��ִ�е�ʱ����ڵ�ǰִ֡�е�ʱ�䣨��ǰ����ˣ�
					if (1000 / 60 - frameTime > 0) {
						Sleep(1000 / 60 - frameTime);
					}
				}
				system("pause");
			}
			else if (m.x >= 155 && m.x <= 325 && m.y >= 360 && m.y <= 410) {
				putimage(0, 0, &img_bk);
				//��һ��Ŀǰû��  loadimage(&img_hp, "��Ϸ����.png", 434, 131);
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
