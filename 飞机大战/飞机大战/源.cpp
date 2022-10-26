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
	int sheild;//����
	bool haveshield;//�ɻ��Ƿ��л���
	int rank;//��rank��������ʾ�÷֣�������Ϊ3��ʱ���ͨ����
	double score;//scoreΪ�÷֣�ÿ����һ�����˵�0.2�֣��÷ִ��ھ��ε�ʱ��˵��������������
	bool isDie;//�ɻ��Ƿ�����
	bool undefeat;//�ɻ��Ƿ����޵�״̬
	int crashtime;//���һ�η���ײ����ʱ��
	int frame;//֡
	int hitframe;//�����������⶯����֡
	int rankframe;//�������εĶ���֡
	

	void setUndefeat();
	void delUndefeat();
}gamer;
Plane enemy[ENEMY_NUM];
struct Bullet {
	int x, y;
	bool isDie;
}bullet[BULLET_NUM];
IMAGE flushimg;		//������,��ֹ����
IMAGE img_gamer[2];//��ҷɻ���ͼƬ
IMAGE img_gamerhit[2];//���ܺľ�ʱ����ҷɻ���ײ��֮���ͼƬ
IMAGE img_sheildhit[5];//���л���ʱ����ҷɻ���ײ��֮���ͼƬ
IMAGE img_bk;		//����ͼ��Ҳ����ͼ��������
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
//�ɻ���ʼ��
void init_Plane(Plane* pthis, int x, int y) {
	pthis->isDie = false;
	pthis->x = x;
	pthis->y = y;
	pthis->frame = 0;
	pthis->hitframe = 0;
	pthis->HP = 6;
	pthis->sheild = 4;
	pthis->haveshield = true;
	pthis->undefeat = false;	//�ʼ�����޵�״̬��ֻ�п���Ѫ��һ��ʱ���ڻ��߼񵽵��߲Ž���
	pthis->rank = 0;
	pthis->score = -1;//!!!!!!!ע������Ҫ��-1��ʼ��������score���ھ��ε�ʱ���������
}
//���Ʒɻ�
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
	gettextstyle(&f);						// ��ȡ��ǰ��������
	f.lfHeight = 48;						// ��������߶�Ϊ 48
	_tcscpy(f.lfFaceName, _T("����"));		// ��������Ϊ�����塱(�߰汾 VC �Ƽ�ʹ�� _tcscpy_s ����)
	f.lfQuality = ANTIALIASED_QUALITY;		// �������Ч��Ϊ�����  
	TCHAR fontt[] = "����";
	TCHAR score[] = "Score ";
	char s[2];
	settextstyle(18, 10, fontt);
	outtextxy(365, 100, score);
	sprintf(s, "%d", pthis->rank);
	outtextxy(420, 100, s);
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
		if (pthis->score > 5) { pthis->isDie = true; break; }
		if (pthis->score >= pthis->rank)pthis->rank++;
		if (pthis->HP == 0) {
			pthis->isDie = true;		
			break;
		}//�������ֵΪ����Ϸ����
		if (pthis->sheild == 0) { //������Ϊ0֮��ʼ��Ѫ
			
			if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->HP--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
		
		}
		else {
			if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y >= enemy[i].y && pthis->y <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x >= enemy[i].x && pthis->x <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			else if (pthis->undefeat == 0 && pthis->x + img_gamer->getwidth() >= enemy[i].x && pthis->x + pthis->x + img_gamer->getwidth() <= enemy[i].x + img_enemy->getwidth() && pthis->y + img_gamer->getheight() >= enemy[i].y && pthis->y + img_gamer->getheight() <= enemy[i].y + img_enemy->getheight()) {
				pthis->sheild--;
				pthis->crashtime = clock();//��¼���һ��ײ��ʱ��
				pthis->setUndefeat();//����Ϊ�����޵�
			}
			
		}
		if (clock() - pthis->crashtime >= 2000)pthis->delUndefeat();		//�޵�ʱ��Ϊ2s
		
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
						//��Ҫ�ӵ÷���������rank
						pthis->score += 0.2;
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
	//�����ӵ�
	loadimage(img_bullet + 0, "bullet1.png");
	loadimage(img_bullet + 1, "bullet2.png");
	loadimage(img_raser+0, "raserbullet0.png");
	loadimage(img_raser + 1, "raserbullet1.png");
	loadimage(img_raser + 2, "raserbullet2.png");
	loadimage(img_raser + 3, "raserbullet3.png");
	loadimage(img_raser + 4, "raserbullet4.png");
	//�л�
	loadimage(img_enemy + 0, "enemy1.png");
	loadimage(img_enemy + 1, "enemy2.png");
	//����ֵ�ͻ�������
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
//���ƽ���
void draw() {
	//����
	putimage(0, 0, &img_bk);
	//���
	draw_Plane(&gamer);
	
	//�ӵ�
	for (int i = 0; i < BULLET_NUM; ++i) {
		if (!bullet[i].isDie) {
			int j = gamer.score;
			drawAlpha(img_raser+j, bullet[i].x, bullet[i].y, 80, 80, 0, 0, 1);
			drawAlpha(img_raser+j, gamer.x, bullet[i].y, 80, 80, 0, 0, 1);
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

void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	if (dstimg == NULL)
	{
		return;
	}
	// ������ʼ��
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = dstimg->getwidth();
	int dst_height = dstimg->getheight();

	// ʵ��͸����ͼ  ���Ż�
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
	//��������
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


	TCHAR s[50] = "1.��ʼ��Ϸ";
	TCHAR s1[50] = "2.��Ϸ����";
	TCHAR s2[50] = "3.�˳�";
	button(155, 210, 170, 50, s);
	button(155, 360, 170, 50, s1);
	button(155, 510, 170, 50, s2);
	MOUSEMSG m;
	while (true) {
		//��������
		mciSendString("open cornfield_chase.mp3 alias mymusic", NULL, 0, NULL);
		mciSendString("play mymusic", NULL, 0, NULL);
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
						HWND wnd = GetHWnd();
						if (MessageBox(wnd, "���Ƿ���Ҫ����һ�֣�", "�ɻ���������", MB_YESNO) == 6)Play();
						else
							return;
					}
					moveBullet();
					//��һ��ʱ�����һ���л�
					if (Timer(200, 1)) {
						createEnemy();
					}
					moveEnemy(&gamer);
					int frameTime = clock() - startTime;
					//һ֡Ӧ��ִ�е�ʱ����ڵ�ǰִ֡�е�ʱ�䣨��ǰ����ˣ�
					if (1000 / 60 - frameTime > 0) {
						Sleep(1000 / 60 - frameTime);
					}

					//˫������������
					drawAlpha(&flushimg, 0, 0, &img_bk);
					drawAlpha(&img_bk, 0, 0, &flushimg);
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
