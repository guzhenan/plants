#include <stdio.h>
#include <graphics.h>
#include "tools.h"
#define WIDTH 900
#define HEIGHT 600
#define MAP_ROW 3
#define MAP_COL 9
enum { SUNFLOWER, PEA, BIG_MOUSE, TOTAL };
IMAGE imgBg; //背景图
IMAGE imgBr; //植物栏目
IMAGE imgCards[TOTAL]; //植物卡片
IMAGE* imgPlants[TOTAL][20]; //植物帧图
char path[64];//图片路径缓冲

struct currentStatus {
	int index = 0;	//当前选中的植物索引,没有则为0
	int curX=0;	//当前选中植物后鼠标的X坐标	
	int curY=0;	//当前选中植物后鼠标的Y坐标	
};
currentStatus current;

struct zhiwu {
	int type;//0代表无植物，1代表选中了第一种植物
	int frameindex;//帧序号，植物
};
struct zhiwu map[MAP_ROW][MAP_COL];

bool is_FileExist(char* path)
{
	FILE* fp = fopen(path, "r");
	if (fp != NULL) {
		fclose(fp);
	}
	return fp != NULL;
}

void gameInit()//初始化
{
	memset(imgPlants, NULL, sizeof(imgPlants));//指针全设为空指针
	memset(map, 0, sizeof(map)); //初始地图都为0
	loadimage(&imgBg, "res/bg.jpg"); //加载背景图，改为多字符集
	loadimage(&imgBr, "res/bar5.png");//加载植物栏
	for (int i = 0; i < TOTAL; ++i) //加载植物卡片
	{
		snprintf(path, sizeof(path), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], path);
		for (int j = 0; i < 20; ++j)
		{
			snprintf(path, sizeof(path), "res/zhiwu/%d/%d.png", i, j + 1);

			if (is_FileExist(path)) {
				imgPlants[i][j] = new IMAGE; //分配内存空间如果文件存在
				loadimage(imgPlants[i][j], path); //加载图片到内存
			}
			else
				break;
		}

	}
	initgraph(WIDTH, HEIGHT, 1);
}

void windowUpdate()//图片渲染
{
	BeginBatchDraw();
	putimage(0, 0, &imgBg);//渲染背景
	putimagePNG(250, 0, &imgBr);//渲染卡片栏
	for (int i = 0; i < TOTAL; ++i)//渲染卡片
	{
		int _X = 330 + 69 * i;
		int _Y = 6;
		putimage(_X, _Y, &imgCards[i]);
	}

	for (int i = 0; i < MAP_ROW; ++i)//渲染种植
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

	if (current.index)//渲染拖动植物
	{
		IMAGE* imgbuffer = imgPlants[current.index - 1][0];
		putimagePNG(current.curX - imgbuffer->getwidth() / 2, current.curY - imgbuffer->getheight() / 2, imgbuffer);
	}
	EndBatchDraw();
}


void gameUpdate()//游戏数据更新
{
	for (int i = 0; i < MAP_ROW; ++i)//帧动更新
	{
		for (int j = 0; j < MAP_COL; ++j)
		{
			if (map[i][j].type != 0)
			{
				int plant_type_index = map[i][j].type - 1;
				map[i][j].frameindex ++;
				int plant_frameindex = map[i][j].frameindex;
				if (imgPlants[plant_type_index][plant_frameindex] == NULL) map[i][j].frameindex = 0; //避免数组边界溢出，帧清零
			}
		}
	}
}

void mouseClick()//用户事件
{
	ExMessage msg;
	static int status = 0;
	static bool flag = true;
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (msg.x > 330 && msg.x < 330 + 69 * TOTAL && msg.y < 96)
			{
				int index = (msg.x - 330) / 65;//索引求解
				current.index = index + 1;//索引赋值
				status = 1; //状态转变
				printf("card_index=%d\n", current.index);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)
		{
			current.curX = msg.x;
			current.curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			status = 0;
			if (current.curY > 170 && current.curY< 490 && current.curX>250 && current.curX < 990)
			{
				
				int row = (current.curY - 170) / 107;
				int col = (current.curX - 250) / 82;
				if (map[row][col].type == 0) //判断当前格子中是否已种植了植物
				{
					map[row][col].type = current.index;
					printf("row=%d col=%d\n", row, col);//调试
					for (int i = 0; i < MAP_ROW; ++i)//调试
					{
						for (int j = 0; j < MAP_COL; ++j)
						{
							if (j != MAP_COL - 1) printf("%d ", map[i][j].type);
							else printf("%d", map[i][j].type);

						}printf("\n");
					}
					
				}
					
				}
			current.index = 0;
		}
	}
}

void startUI()//开始界面
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