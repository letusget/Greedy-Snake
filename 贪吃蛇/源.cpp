#define _CRT_SECURE_NO_WARNINGS 1
#undef UNICODE
#undef _UNICODE
/*
�Լ���������
���Ͻ�Ϊ��0��0��������Ϊx�ᣬ����Ϊy��

*/

#include <stdio.h>
#include <Windows.h>
#include <graphics.h>
#include <time.h>
#include <stdlib.h>

#include <mmsyscom.h>//�����˿��Բ������ֵ�ͷ�ļ�
#pragma comment(lib,"winmm.lib")//���ļ�

#include <conio.h>
//�����ͼ�λ��Ĵ��ڵ�ͷ�ļ�
//��Ҫ���а�װͷ�ļ�
//Ҫע�� easyx ��Ҫ��c++������ʹ�ã�����������ӵ��ļ�Ҫʹ�á�.cpp����ʽ

//�ú궨���˴��ڵ���ʾ���
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define MAX_SNAKE 100  //�ߵ�������

enum DIR
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	/*
	enum��ö�����ͣ�����c�����е�һ���������͡��ڡ�ö�١����͵Ķ������о�ר�����п��ܵ���ȡֵ��
	��˵��Ϊ�á�ö�١����͵ı���ȡֵ���ܳ�������ķ�Χ��ö��������һ�ֻ����������ͣ�
	������һ�ֹ������ͣ���Ϊ�������ٷֽ�Ϊ�κλ������͡�
	�����������г����п��ܵ�ȡֵ����������ȡһ�����֡�������������Ҿ���ö��������������
	�ο���http://c.biancheng.net/view/2034.html
	*/
};

struct Snake_tlg  //�ߵĽṹ��
{
	int num;  // ����ĳ���
	int dir;  //�ߵķ���
	int score;  //����
	int size;  //�ߵĳߴ磬����͸߶�
	POINT coor[MAX_SNAKE];  //�ߵ�λ�����ꡣһ���ṹ������
	//POINT Ҳʱһ���ṹ�壬������ͨ��ѡ��ת���������鿴�����ṹ������ windef.h�е�
	/*
	typedef struct tagPOINT
	{
    LONG  x;
    LONG  y;
	} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;
	*/

}snake;

struct Food_tlg  //ʳ��ĳ��� �ṹ��
{
	POINT fd;  //ʳ�������
	int flag;  //�Ƿ��Ѿ����߳Ե�
	DWORD color;  //����ʳ�����ɫ
	/*��DirectX��������Щ�ط���DWORD(RGBA)��ʽ��ʾ��ɫ���䷶Χ��[0 - 255]*/
}food;

void GameInit();  //��ʼ����ϷԪ��
void GameDraw();  //����̰����
void SnakeMove();  // �����ߵ��ƶ�
void KeyControl();  //���Ʒ�����
void EatFood();  //��ʳ��
void DontEatSelf();  //��Ϸ�����ж�
 void Pause();  //��Ϸ��ͣ

void main(void)
{
	initgraph(WIN_WIDTH,WIN_HEIGHT,SHOWCONSOLE);//��ʼ��һ��ͼ�δ���
	//�����������https://docs.easyx.cn/zh-cn/initgraph
	printf("\tWelcome! \n");
	printf("\tPlay Game Right Now��\n");
	GameInit();  //��ʼ����ϷԪ��
	//DWORD t1, t2;  //�����ٶ�
	//BeginBatchDraw();
	//t1 = t2 = GetTickCount;
	while (1)  //ʹ����ѭ������֤����һֱ����
	{
		/*if (t2 - t1 > 100)
		{
			SnakeMove();
			t1 = t2;
		}
		t2 = GetTickCount;*/

		SnakeMove();  //���ƶ�
		GameDraw();  //�ٻ���
		EatFood();
		//FlushBatchDraw();  //���������������
		KeyControl(); 
		DontEatSelf();
		Sleep(50);  //ÿ�ζ�Ҫ��������ӣ��ſ���ʵʱ�Ŀ��������������ٶ�̫���ˣ�û������
		//ͬ���������Ҳ���Գ�Ϊ������Ϸ�Ѷȵ�ѡ��
		//Pause();

	}
	//GameDraw();
	//SnakeMove();

	getchar();//������ֹ ��Ϸ����һ������
	closegraph();  //�ر���Ϸͼ�δ���
}

void GameInit()
{
	srand((unsigned int)time(NULL));	//time ��ͷ�ļ��� time.h

	//������
	snake.num = 3;  //�����߳�ʼ����������
	snake.dir = RIGHT;  //��ʼʱ��ͷ����
	snake.score = 0;  //��ʼʱ����Ϊ0
	snake.size = 10;  //��ʼ���ߵ������С
	snake.coor[2].x = 0;  //���������壨β�ͣ�������
	snake.coor[2].y = 0;  // ���ڳ�ʼ��ʱ���������Ͻǵ�һ������������y����0
	snake.coor[1].x = 0+snake.size;  // �ߵĵڶ��ڣ����������
	snake.coor[1].y = 0;
	snake.coor[0].x = 0+2*snake.size;  //��ͷ������һ������
	snake.coor[0].y = 0;

	//��ʼ��ʳ��
	food.fd.x = rand() % (WIN_WIDTH/10)*10;  //ȡģȷ�����������ֵ�ڷ�Χ��
	food.fd.y = rand() % (WIN_HEIGHT/10)*10;
//����Ҫ�ȶ�10ȡ�̣��ٳ�10��Ϊ�˲���һ��10�ı�������������ͷ��ʳ��Ӵ�����������ÿ�ζ���һ����������޷��ж�
	food.flag = 1;  //��ʼ��ʳ�����
	food.color = RGB(rand() % 256, rand() % 256, rand() % 256);//�������ʳ����ɫ

	mciSendString("open Carnival_De_Brazil.mp3 alias a",0,0,0);
	mciSendString("play a repeat",0,0,0);

}


void GameDraw()
{
	setbkcolor(RGB(105, 160, 141));  //���ñ�����ɫ,��web�����е���ɫ����һ��
	cleardevice();  //�������Ԫ��

	//����
	setfillcolor(YELLOW);
	int i;
	for (i = 0; i < snake.num; i++)  //ÿ�ζ���һ�����Σ�������
	{
		setlinecolor(BLACK);
		fillrectangle(snake.coor[i].x,
			snake.coor[i].y,
			snake.coor[i].x + snake.size,
			snake.coor[i].y + snake.size);
		/*�� easyx �У�solidrectangle ����������ڻ��ޱ߿��������,��fillrectangle���ڻ��б߿��������
	void solidrectangle(int left,int top,int right,int bottom);
	void solidrectangle(������ x ����,	���ζ��� y ����,	�����Ҳ� x ����,	���εײ� y ����);
		*/

		//printf("%d %d\n", snake.coor[i].x, snake.coor[i].y);  //����Ч������,�ڵ���ʱʹ��
	}

	//��ʳ��
	if (food.flag == 1) //Ҫȷ����ʳ�ﱻ����֮�����ʧ����
	{

		setfillcolor(food.color);  //����ʳ����ɫ
		fillellipse(food.fd.x, food.fd.y, food.fd.x + 10, food.fd.y + 10);  //����ʳ����״
	}

	//��ʾ����
	char temp[20] = "";
	sprintf(temp,"������%d", snake.score);
	//SetBkMode(TRANSPARENT);
	outtextxy(20, 20, temp);
	//������һ���ֵ���ϸ���Ͳμ���https://codeabc.cn/yangw/a/error-c2665

}

void KeyControl()
{
	//ʹ��win32API����ȡ������Ϣ�����ʹ���ַ��������洢��ת��ΪASCIIҪ��ݺܶ�
	/*
	GetAsyncKeyState������ʹ�õ��ã���Ӧ�����Ǵ��ڰ��»��ǵ���״̬
	���������óɹ�������ֵָ�������ϴε���GetAsyncKeyState������ָ���İ����Ƿ��ڰ���״̬��
	�Լ��ü������Ǵ��ڰ��»��ǵ�����������λ����1�������������£�
	��������λ����1���������ǰ�ε���GetAsyncKeyStateһֱ���ڰ���״̬��
	������òο���https://blog.csdn.net/sunone_/article/details/77461067
	*/

	if (GetAsyncKeyState(VK_UP)&&snake.dir!=DOWN)//���������ȷ���˲����Խ��з������
	{
		snake.dir = UP;
	}
	if (GetAsyncKeyState(VK_DOWN) && snake.dir != UP)
	{
		snake.dir = DOWN;
	}
	if (GetAsyncKeyState(VK_LEFT) && snake.dir != RIGHT)
	{
		snake.dir = LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT) && snake.dir != LEFT)
	{
		snake.dir = RIGHT;
	}

}

void SnakeMove()
{
	/*Ҫ������ͷ���ƶ����ߵ�����������ͷһ���˶�������
	Ҫ�����һ�ڿ�ʼ�����ߵ�ÿһ�����嶼�����ƶ�����ǰһ�������λ����
	*/
	int i;
	for (i = snake.num - 1; i > 0; i--)  //��Ϊ�Ƕ�������в�������������Ҫ -1,����������i-- 
	{
		snake.coor[i].x = snake.coor[i - 1].x;
		snake.coor[i].y = snake.coor[i - 1].y;

	}
	switch (snake.dir)
	{
	case UP:snake.coor[0].y -= 10;
		/*
		����Ҫʵ�ֵ��ǿ����ڴ��ڴ����ɽ���
		��Ϊ��¼������ͷ������ߵ����꣬����������Ҫʹ��ͷ�ڽӴ�������(����ͷ���ұߣ��ͽ�������ı߽�
		��������Ҫ�и�+10������Ϊ��ȥ����ͷ����ĳ���
		*/
		if (snake.coor[0].y +10 <= 0) 
		{
			snake.coor[0].y = WIN_HEIGHT;
		}
		break; 
	case DOWN:snake.coor[0].y += 10;
		if (snake.coor[0].y  >= WIN_HEIGHT)
		{
			snake.coor[0].y = 0;
		}
		break;
	case LEFT:snake.coor[0].x -= 10;
		if (snake.coor[0].x + 10 <= 0)
		{
			snake.coor[0].x = WIN_WIDTH;
		}
		break;
	case RIGHT:snake.coor[0].x += 10; 
		if (snake.coor[0].x  >= WIN_WIDTH)
		{
			snake.coor[0].x = 0;
		}
		break;

	default:printf("��������������²�����"); 
		break;
	}
	//snake.coor[0].x += 10;

}

void EatFood()
{
	//�ж���ͷ��ʳ�������Ϳ�����
	if (snake.coor[0].x == food.fd.x && snake.coor[0].y == food.fd.y&&food.flag==1)
	{
		snake.num++;
		snake.score += 10;
		food.flag = 0;   //�ڳ���֮�󣬾���ʳ�ﲻ����
	}
	if (food.flag == 0)  //��ʳ�����֮��Ϳ��Լ�������ʳ����
	{
		food.fd.x = rand() % (WIN_WIDTH / 10) * 10;  
		food.fd.y = rand() % (WIN_HEIGHT / 10) * 10;
		food.flag = 1; 
		food.color = RGB(rand() % 256, rand() % 256, rand() % 256);
	}

}

void DontEatSelf()
{
	for (int i = 4; i < snake.num; i++)
	{
		if (snake.coor[0].x == snake.coor[i].x && snake.coor[0].y == snake.coor[i].y)
		{
			outtextxy(200, 200, "Game over!");
			_getch();  //����ͣ��
			exit(666);
		}

	}
}

void Pause()
{
	if (_getch() == 32)  //32��ʾ�ո�
	{
		//while (_getch() != ' ');
		while (1)
		{
			_getch();
		}
	}
	
}


