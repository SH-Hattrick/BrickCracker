#include<iostream>
#include<vector>
#include<cmath>
#include<easyx.h>
#include<graphics.h>
#include<map>
#include<iomanip>
#include<algorithm>
#include<conio.h>
#include<map>
#include<vector>
#include<ctime>
#include<iomanip>
//#include"Botton.h"
using namespace std;

#define original_x 0
#define original_y 0
#define BRICK_CRACKED -10000
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
//#define UNICODE Unicode



IMAGE BEGIN;
IMAGE BRICKS[5];
IMAGE BLANK;
IMAGE BALL;
IMAGE THE_PAT;


void generate_graph();
void generate_init_graph();
void game_on();
void goodbye();
class ball;
class bricks;
class Pat;


static int Mymap[960][600]{ 0 };
vector<bricks> mybricks;


class ball
{
private:
	int x_speed = 3;
	int y_speed = 3;
	int x_angle = -1;
	int y_angle = -1;
	int _x = original_x;
	int _y = original_y;
	int radius = 10;
public:
	ball() {};
	ball(int x, int y) { _x = x; _y = y; }
	friend bool if_collide(ball* ball);
	friend bool if_bounce(ball* ball, Pat* pat);
	void SetBallPosition(int x,int y)
	{
		_x = x; _y = y;
		setlinecolor(BLUE);
		setfillcolor(YELLOW);
		fillcircle(_x, _y, radius);
	}//set original position
	void drawball()
	{
		setlinecolor(BLUE);
		setfillcolor(YELLOW);
		fillcircle(_x, _y, radius);
	}//draw ball at new position
	void vanish_ball()
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		fillcircle(_x, _y, radius);
	}//clear original position
	void Move_the_Ball()
	{
		_x += x_angle * x_speed;
		_y += y_angle * y_speed;
		drawball();
	}//change attributes of the ball
	void change_dir(Pat* pat)
	{
		if (_x + radius > 960 || _x + radius < 0)
			x_rev();
		//collide against walls
		else if (_y - radius < 0)
			y_rev();
		else if (if_collide(this))
			y_rev();
		else if (if_bounce(this, pat))
			y_rev();
		return;
	}//ball original position:480 550
	void y_rev()
	{
		y_angle = -y_angle;
	}//竖直移动方向改变
	void x_rev()
	{
		x_angle = -x_angle;;
	}//水平移动方向改变
	bool Game_over()
	{
		if (_y > 600 || mybricks.empty())
		{
			Sleep(2000); 
			return true;
		}
		else return false;
	}//判断球是否“掉落”
};


class bricks
{
private:
	int level = 1;
	int _x = original_x;
	int _y = original_y;
	int x_size = 96;
	int y_size = 40;
protected:
	int bottomline = BRICK_CRACKED;
	int leftmargin = BRICK_CRACKED;
	int rightmargin = BRICK_CRACKED;
public:
	bricks() {}
	bricks(int i) { level = i; }
	friend bool if_collide(ball* ball);
	void reset()
	{
		_x = BRICK_CRACKED;
		_y = BRICK_CRACKED;
		bottomline = BRICK_CRACKED;
		rightmargin = BRICK_CRACKED;
		leftmargin = BRICK_CRACKED;
	}//初始化重设属性
	void breakdown()
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		if (_x != BRICK_CRACKED)
			for (int i = _x; i < _x + x_size; ++i)
				for (int j = _y; j < _y + y_size; ++j)
					Mymap[i][j] = 0;
		fillrectangle(_x, _y, _x + x_size, _y + y_size);
		reset();
		//replace image of a brick with blank
	}//When deconstructing a brick, its visual image must as well disappear
	void setBrickPosition(int x, int y)
	{
		_x = x; _y = y;
		bottomline = _y + y_size;
		leftmargin = _x;
		rightmargin = _x + x_size;
		DrawBrick();
		for (int i = _x; i < _x + x_size; ++i)
			for (int j = 0; j < _y + y_size; ++j)
				Mymap[i][j] = 1;
		mybricks.push_back(*this);
	}//构建砖块
	void DrawBrick()
	{
		setlinecolor(BLACK);
		setfillcolor(RED);
		fillrectangle(_x, _y, _x + x_size, _y + y_size);
	}//called when drawing a brick
};


class Pat
{
private:
	int _x = original_x;
	int _y = original_y;
	int length = 75;
	int thickness = 15;
	int speed = 12;
public:
	Pat() {};
	Pat(int x, int y) { _x = x; _y = y; }
	friend bool if_bounce(ball* ball, Pat* pat);
	void left_move()
	{
		if (_x < 0)
			return;
		vanish_pat();
		_x -= speed;
		draw_pat();
	}//
	void right_move()
	{
		if (_x + length > 960)
			return;
		vanish_pat();
		_x += speed;
		draw_pat(); 
	}//右移
	void draw_pat()
	{
		setlinecolor(BLACK);
		setfillcolor(MAGENTA);
		fillrectangle(_x, _y, _x + length, _y + thickness);
	}//左移
	void vanish_pat()
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		fillrectangle(_x, _y, _x + length, _y + thickness);
	}//清除原位置
};



void generate_graph(ball* ball, bricks bri[], int num)
{
	setbkcolor(WHITE);
	cleardevice();
	for (int i = 0; i <= 960 - 96; i += 96)
		for (int j = 0; j < 200; j += 40)
		{
			bri[j/40 + (i / 96) * (j / 40)].setBrickPosition(i, j);
			//bri[j/40 + (i / 96) * (j / 40)].DrawBrick();
		}
	for (int i = 600; i > 570; --i)
	{
		setlinecolor(RGB((-i + 600) * 3, 0, (-i + 600) * 2));
		line(0, i, 960, i);
	}
	ball->SetBallPosition(480, 545);
}//Game static intrface



void generate_init_graph()
{
	HWND hwnd = GetHWnd();
	SetWindowText(hwnd, _T("Bricks Cracker"));
	loadimage(&BEGIN, _T("begin.jpg"), 960, 600, false);
	putimage(0, 0, &BEGIN);
}//Before entering the game



bool if_collide(ball* ball)
{
	for (vector<bricks>::iterator a = mybricks.begin(); a < mybricks.end(); ++a)
	{
		if ((ball->_y < a->bottomline) &&
			(ball->_x < a->rightmargin) &&
			(ball->_x > a->leftmargin))
		{  
			a->breakdown();
			mybricks.erase(a);
			return true;
		}
	}//单层循环判断是否撞击砖块
	return false;
}


bool if_bounce(ball* ball, Pat* pat)
{
	if (ball->_x + ball->radius<pat->_x + pat->length &&
		ball->_x + ball->radius>pat->_x&& ball->y_angle > 0 &&
		ball->_y + ball->radius > pat->_y)
		return true;
	else return false;
}//判断是否出现撞击板而反弹



void goodbye()
{
	setbkcolor(WHITE);
	cleardevice();   
	settextcolor(DARKGRAY);
	outtextxy(380, 300, _T("GoodBye!!!"));
	Sleep(2000);
}//结束界面



void remind_info()
{
	setbkcolor(WHITE);
	cleardevice();
	settextcolor(DARKGRAY);
	outtextxy(350, 300, _T("Press anykey to continue...Press ESC to exit..."));
	outtextxy(400, 375, _T("Press ESC to exit"));
}//提示信息界面



void game_on(ball* ball)
{
	int index = 0;
	char Userkey = _getch();
	Pat* pat = new Pat{ 480,548 };
	pat->draw_pat();
	while (Userkey != 27)
	{
		ball->vanish_ball();
		ball->change_dir(pat);
		ball->Move_the_Ball();
		Sleep(8);
		
		//下面实现光标控制方向
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			if (msg.mkLButton)
				pat->left_move();
			else if (msg.mkRButton)
				pat->right_move();
			FlushMouseMsgBuffer();
		}
		//CTRL暂停游戏

		if (_kbhit())
		{
			Userkey = _getch();
			switch (Userkey)
			{
			case 'd':
			case 'D':
			case VK_RIGHT:
				pat->right_move();
				break;
			case 'a':
			case 'A':
			case VK_LEFT:
				pat->left_move();
				break;
			}
		}
		if (ball->Game_over())
			break;
		/*
		if (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();//鼠标信号:阻塞函数
			if (msg.mkRButton)//若点击右键
			{
				settextcolor(WHITE);
				outtextxy(400, 580, _T("I am a GENIUS"));
				//若干操作
			}
		}
		FlushMouseMsgBuffer();//清空缓冲区，防止闪屏
		*/
	}//主要函数
}



bricks bri[300]{};//砖块数组
int main()
{
	initgraph(960, 600);
	generate_init_graph();
	char c = _getch();
	while (true)
	{
		ball ball{};
		generate_graph(&ball, bri, 300);
		game_on(&ball);
		remind_info();
		c = _getch();
		if (c == 27)
		{
			goodbye();
			break;
		}
		generate_init_graph();
	}
	closegraph();
	return 0;
}