#include <stdio.h>
#include <graphics.h>
#include "tools.h"
#define WIDTH 900
#define HEIGHT 600
#define MAP_ROW 3
#define MAP_COL 9
enum { SUNFLOWER, PEA, BIG_MOUSE, TOTAL };
IMAGE imgBg; //����ͼ
IMAGE imgBr; //ֲ����Ŀ
IMAGE imgCards[TOTAL]; //ֲ�￨Ƭ
IMAGE* imgPlants[TOTAL][20]; //ֲ��֡ͼ
char path[64];//ͼƬ·������

struct currentStatus {
	int index = 0;	//��ǰѡ�е�ֲ������,û����Ϊ0
	int curX=0;	//��ǰѡ��ֲ�������X����	
	int curY=0;	//��ǰѡ��ֲ�������Y����	
};
currentStatus current;

struct zhiwu {
	int type;//0������ֲ�1����ѡ���˵�һ��ֲ��
	int frameindex;//֡��ţ�ֲ��
};
struct zhiwu map[MAP_ROW][MAP_COL];

bool is_FileExist(char* path)//
{
	FILE* fp = fopen(path, "r");
	if (fp != NULL) {
		fclose(fp);
	}
	return fp != NULL;
}

void gameInit()//��ʼ��
{
	memset(imgPlants, NULL, sizeof(imgPlants));//ָ��ȫ��Ϊ��ָ��
	memset(map, 0, sizeof(map)); //��ʼ��ͼ��Ϊ0
	loadimage(&imgBg, "res/bg.jpg"); //���ر���ͼ����Ϊ���ַ���
	loadimage(&imgBr, "res/bar5.png");//����ֲ����
	for (int i = 0; i < TOTAL; ++i) //����ֲ�￨Ƭ
	{
		snprintf(path, sizeof(path), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], path);
		for (int j = 0; j < 20; ++j)
		{
			snprintf(path, sizeof(path), "res/zhiwu/%d/%d.png", i, j + 1);

			if (is_FileExist(path)) {
				imgPlants[i][j] = new IMAGE; //�����ڴ�ռ�����ļ�����
				loadimage(imgPlants[i][j], path); //����ͼƬ���ڴ�
			}
			else
				break;
		}

	}
	initgraph(WIDTH, HEIGHT, 1);
}

void windowUpdate()//ͼƬ��Ⱦ
{
	BeginBatchDraw();
	putimage(0, 0, &imgBg);//��Ⱦ����
	putimagePNG(250, 0, &imgBr);//��Ⱦ��Ƭ��
	for (int i = 0; i < TOTAL; ++i)//��Ⱦ��Ƭ
	{
		int _X = 330 + 69 * i;
		int _Y = 6;
		putimage(_X, _Y, &imgCards[i]);
	}

	for (int i = 0; i < MAP_ROW; ++i)//��Ⱦ��ֲ
	{
		for (int j = 0; j < MAP_COL; ++j)
		{
			if (map[i][j].type != 0)
			{
				int plant_type_index = map[i][j].type - 1;
				int plant_frameindex = map[i][j].frameindex;
				putimagePNG(250+j*82,176+i*108 ,imgPlants[plant_type_index][plant_frameindex]);
			}

		}
	}

	if (current.index)//��Ⱦ�϶�ֲ��
	{
		IMAGE* imgbuffer = imgPlants[current.index - 1][0];
		putimagePNG(current.curX - imgbuffer->getwidth() / 2, current.curY - imgbuffer->getheight() / 2, imgbuffer);
	}
	EndBatchDraw();
}


void gameUpdate()//��Ϸ���ݸ���
{
	for (int i = 0; i < MAP_ROW; ++i)//֡������
	{
		for (int j = 0; j < MAP_COL; ++j)
		{
			if (map[i][j].type != 0)
			{
				int plant_type_index = map[i][j].type - 1;
				map[i][j].frameindex ++;
				int plant_frameindex = map[i][j].frameindex;
				if (imgPlants[plant_type_index][plant_frameindex] == NULL) map[i][j].frameindex = 0; //��������߽������֡����
			}
		}
	}
}

void mouseClick()
{
	ExMessage msg;
	if (peekmessage(&msg))
	{
		// 处理卡片区域的点击
		if (msg.message == WM_LBUTTONDOWN)
		{
			// 检查是否点击了植物卡片区域
			if (msg.x > 330 && msg.x < 330 + 69 * TOTAL && msg.y < 96)
			{
				int index = (msg.x - 330) / 65;
				current.index = index + 1;
				printf("card_index=%d\n", current.index);
			}
			// 检查是否在有效的种植区域点击
			else if (current.index > 0 && 
					 msg.y > 170 && msg.y < 490 && 
					 msg.x > 250 && msg.x < 990)
			{
				int row = (msg.y - 170) / 107;
				int col = (msg.x - 250) / 82;
				if (map[row][col].type == 0) // 确保该位置没有植物
				{
					map[row][col].type = current.index;
					map[row][col].frameindex = 0;
					current.index = 0; // 重置选择状态
					
					// 调试输出
					printf("row=%d col=%d\n", row, col);
					for (int i = 0; i < MAP_ROW; ++i)
					{
						for (int j = 0; j < MAP_COL; ++j)
						{
							if (j != MAP_COL - 1) printf("%d ", map[i][j].type);
							else printf("%d", map[i][j].type);
						}
						printf("\n");
					}
				}
			}
			// 如果点击其他区域，取消选择
			else
			{
				current.index = 0;
			}
		}
		// 处理鼠标移动
		else if (msg.message == WM_MOUSEMOVE && current.index > 0)
		{
			current.curX = msg.x;
			current.curY = msg.y;
		}
	}
}

void startUI()//ʼ
{
	IMAGE imgUIbg, imgUI_mu1, imgUI_mu2; 
	loadimage(&imgUIbg,"res/menu.png");
	loadimage(&imgUI_mu1, "res/menu1.png");
	loadimage(&imgUI_mu2, "res/menu2.png");
	bool flag=true;
	ExMessage msgUI;
	while (1)
	{
		BeginBatchDraw();
		putimage(0, 0, &imgUIbg);
		putimagePNG(484, 70, flag? &imgUI_mu1: &imgUI_mu2);
		if (peekmessage(&msgUI))
		{
			if (msgUI.x > 484 && msgUI.x < 815 && msgUI.y>70 && msgUI.y < 215)
			{
				if (msgUI.message == WM_LBUTTONDOWN) flag = false;
				else if (msgUI.message == WM_LBUTTONUP) return;
			}
			
		}
		EndBatchDraw();
	}

}

int main(void)
{
	gameInit();
	startUI();
	printf("game is inited\n");
	int timer = 0;
	bool flag = true;
	while (TRUE) {
		mouseClick();
		timer+=getDelay();
		if (timer > 60) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			windowUpdate();
			gameUpdate();
		}
	}

	system("pause");
	return 0;
}