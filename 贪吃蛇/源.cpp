#define _CRT_SECURE_NO_WARNINGS 1
#undef UNICODE
#undef _UNICODE
/*
自己创建窗口
左上角为（0，0），横着为x轴，竖着为y轴

*/

#include <stdio.h>
#include <Windows.h>
#include <graphics.h>
#include <time.h>
#include <stdlib.h>

#include <mmsyscom.h>//包含了可以播放音乐的头文件
#pragma comment(lib,"winmm.lib")//库文件

#include <conio.h>
//这个是图形化的窗口的头文件
//需要自行安装头文件
//要注意 easyx 需要在c++环境下使用，所以这里添加的文件要使用“.cpp”格式

//用宏定义了窗口的显示宽高
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define MAX_SNAKE 100  //蛇的最大节数

enum DIR
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	/*
	enum是枚举类型，属于c语言中的一种数据类型。在“枚举”类型的定义中列举专出所有可能的属取值，
	被说明为该“枚举”类型的变量取值不能超过定义的范围。枚举类型是一种基本数据类型，
	而不是一种构造类型，因为它不能再分解为任何基本类型。
	可以用它来列出所有可能的取值，并给它们取一个名字。这里的上下左右就用枚举类型来定义了
	参考：http://c.biancheng.net/view/2034.html
	*/
};

struct Snake_tlg  //蛇的结构体
{
	int num;  // 身体的长度
	int dir;  //蛇的方向
	int score;  //分数
	int size;  //蛇的尺寸，身体和高度
	POINT coor[MAX_SNAKE];  //蛇的位置坐标。一个结构体数组
	//POINT 也时一个结构体，，可以通过选中转到定义来查看其具体结构，是在 windef.h中的
	/*
	typedef struct tagPOINT
	{
    LONG  x;
    LONG  y;
	} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;
	*/

}snake;

struct Food_tlg  //食物的出现 结构体
{
	POINT fd;  //食物的坐标
	int flag;  //是否已经被蛇吃掉
	DWORD color;  //定义食物的颜色
	/*在DirectX程序中有些地方用DWORD(RGBA)格式表示颜色，其范围是[0 - 255]*/
}food;

void GameInit();  //初始化游戏元素
void GameDraw();  //创建贪吃蛇
void SnakeMove();  // 操作蛇的移动
void KeyControl();  //控制方向函数
void EatFood();  //吃食物
void DontEatSelf();  //游戏结束判断
 void Pause();  //游戏暂停

void main(void)
{
	initgraph(WIN_WIDTH,WIN_HEIGHT,SHOWCONSOLE);//初始化一个图形窗口
	//具体参数见：https://docs.easyx.cn/zh-cn/initgraph
	printf("\tWelcome! \n");
	printf("\tPlay Game Right Now！\n");
	GameInit();  //初始化游戏元素
	//DWORD t1, t2;  //控制速度
	//BeginBatchDraw();
	//t1 = t2 = GetTickCount;
	while (1)  //使用死循环来保证可以一直游玩
	{
		/*if (t2 - t1 > 100)
		{
			SnakeMove();
			t1 = t2;
		}
		t2 = GetTickCount;*/

		SnakeMove();  //先移动
		GameDraw();  //再画蛇
		EatFood();
		//FlushBatchDraw();  //这个函数看到很蒙
		KeyControl(); 
		DontEatSelf();
		Sleep(50);  //每次都要间隔半秒钟，才可以实时的看见，否则运行速度太快了，没法操作
		//同理这个操作也可以成为设置游戏难度的选项
		//Pause();

	}
	//GameDraw();
	//SnakeMove();

	getchar();//用来防止 游戏窗口一闪而过
	closegraph();  //关闭游戏图形窗口
}

void GameInit()
{
	srand((unsigned int)time(NULL));	//time 的头文件是 time.h

	//定义蛇
	snake.num = 3;  //定义蛇初始有三节身体
	snake.dir = RIGHT;  //开始时蛇头向右
	snake.score = 0;  //开始时分数为0
	snake.size = 10;  //初始化蛇的身体大小
	snake.coor[2].x = 0;  //第三节身体（尾巴）的坐标
	snake.coor[2].y = 0;  // 由于初始化时就是在左上角的一个长条，所以y都是0
	snake.coor[1].x = 0+snake.size;  // 蛇的第二节（身体的坐标
	snake.coor[1].y = 0;
	snake.coor[0].x = 0+2*snake.size;  //蛇头，即第一节身体
	snake.coor[0].y = 0;

	//初始化食物
	food.fd.x = rand() % (WIN_WIDTH/10)*10;  //取模确保产生的随机值在范围内
	food.fd.y = rand() % (WIN_HEIGHT/10)*10;
//这里要先对10取商，再乘10是为了产生一个10的倍数，可以让蛇头和食物接触到，而不是每次都是一个随机数，无法判断
	food.flag = 1;  //初始化食物存在
	food.color = RGB(rand() % 256, rand() % 256, rand() % 256);//随机产生食物颜色

	mciSendString("open Carnival_De_Brazil.mp3 alias a",0,0,0);
	mciSendString("play a repeat",0,0,0);

}


void GameDraw()
{
	setbkcolor(RGB(105, 160, 141));  //设置背景颜色,与web开发中的颜色设置一样
	cleardevice();  //清除其他元素

	//画蛇
	setfillcolor(YELLOW);
	int i;
	for (i = 0; i < snake.num; i++)  //每次都画一个矩形，共三次
	{
		setlinecolor(BLACK);
		fillrectangle(snake.coor[i].x,
			snake.coor[i].y,
			snake.coor[i].x + snake.size,
			snake.coor[i].y + snake.size);
		/*在 easyx 中，solidrectangle 这个函数用于画无边框的填充矩形,而fillrectangle用于画有边框的填充矩形
	void solidrectangle(int left,int top,int right,int bottom);
	void solidrectangle(矩形左部 x 坐标,	矩形顶部 y 坐标,	矩形右部 x 坐标,	矩形底部 y 坐标);
		*/

		//printf("%d %d\n", snake.coor[i].x, snake.coor[i].y);  //用于效验坐标,在调试时使用
	}

	//画食物
	if (food.flag == 1) //要确保在食物被吃完之后就消失不见
	{

		setfillcolor(food.color);  //设置食物颜色
		fillellipse(food.fd.x, food.fd.y, food.fd.x + 10, food.fd.y + 10);  //设置食物形状
	}

	//显示分数
	char temp[20] = "";
	sprintf(temp,"分数：%d", snake.score);
	//SetBkMode(TRANSPARENT);
	outtextxy(20, 20, temp);
	//关于这一部分的详细解释参见：https://codeabc.cn/yangw/a/error-c2665

}

void KeyControl()
{
	//使用win32API来获取键盘消息，相比使用字符数组来存储并转换为ASCII要快捷很多
	/*
	GetAsyncKeyState函数的使用调用，相应按键是处于按下还是弹起状态
	若函数调用成功，返回值指定了自上次调用GetAsyncKeyState以来，指定的按键是否处于按下状态，
	以及该键现在是处于按下还是弹出。如果最高位被置1，表明键被按下，
	如果是最低位被置1，则表明自前次调用GetAsyncKeyState一直处于按下状态。
	虚拟调用参考：https://blog.csdn.net/sunone_/article/details/77461067
	*/

	if (GetAsyncKeyState(VK_UP)&&snake.dir!=DOWN)//后面的条件确保了不可以进行反向操作
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
	/*要操作蛇头的移动，蛇的身体随着蛇头一起运动，就是
	要从最后一节开始，让蛇的每一节身体都随着移动到它前一节身体的位置上
	*/
	int i;
	for (i = snake.num - 1; i > 0; i--)  //因为是对数组进行操作，所以这里要 -1,并且这里是i-- 
	{
		snake.coor[i].x = snake.coor[i - 1].x;
		snake.coor[i].y = snake.coor[i - 1].y;

	}
	switch (snake.dir)
	{
	case UP:snake.coor[0].y -= 10;
		/*
		这里要实现的是可以在窗口处自由进出
		因为记录的是蛇头方块左边的坐标，而这里我们要使蛇头在接触到边沿(是蛇头的右边）就进入下面的边界
		所以这里要有个+10，就是为了去掉蛇头自身的长度
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

	default:printf("错误操作，请重新操作！"); 
		break;
	}
	//snake.coor[0].x += 10;

}

void EatFood()
{
	//判断蛇头和食物的坐标就可以了
	if (snake.coor[0].x == food.fd.x && snake.coor[0].y == food.fd.y&&food.flag==1)
	{
		snake.num++;
		snake.score += 10;
		food.flag = 0;   //在吃完之后，就让食物不存在
	}
	if (food.flag == 0)  //当食物吃完之后就可以继续产生食物了
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
			_getch();  //保持停顿
			exit(666);
		}

	}
}

void Pause()
{
	if (_getch() == 32)  //32表示空格
	{
		//while (_getch() != ' ');
		while (1)
		{
			_getch();
		}
	}
	
}


