#include <easyx.h>
#include <conio.h>
#include <cstdio>
#include <iostream>
#include <graphics.h>
#include <mmsystem.h>
#include<windows.h>
#include<cmath>
#include<fstream>
#include<random>
#include<ctime>
#include<vector>
#include<cmath>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")

#define BOARD_SIZE 15//棋盘尺寸
const int R = 19;//棋子半径

//用来更换页面
enum page {
	ftStart,
	jxStart,
	ftready,
	jxready,

	ftSetting,
	jxSetting,

	ftRule,
	jxRule,

	ftGame,
	jxGame,
	ftstop,
	jxstop
};
page PAGE = ftStart;

enum state {
	empty,//0
	blackc,//1
	whitec,//2
}BOARD[226][BOARD_SIZE][BOARD_SIZE];//棋局

//记录落子
int placex[226] = { 0 };
int placey[226] = { 0 };

//记录赢棋
int winx[5] = { 0 };
int winy[5] = { 0 };

//初始化棋盘
void initialize() {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			BOARD[0][i][j] = empty;
		}
	}
}

void initialize_k (int k) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			BOARD[k][i][j] = empty;
		}
	}
}

int step = 0;//棋的步数
bool end = false;//是否结束

//判断胜利
bool Win(bool turn) {
	//0 blackc,1 whitec
	state color;
	if (turn) color = whitec;
	else color = blackc;
	//横向
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					for (int k = 0; k < 5; ++k) {
						winx[k] = i;
						winy[k] = j;
						--j;
					}
					return true;
				}
			}
			else {
				count = 0;
			}
		}
	}
	//纵向
	for (int j = 0; j < BOARD_SIZE; ++j) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			if (BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					for (int k = 0; k < 5; ++k) {
						winx[k] = i;
						winy[k] = j;
						--i;
					}
					return true;
				}
			}
			else {
				count = 0;
			}
		}
	}
	//左下右上
	for (int k = 4; k < 25; ++k) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			int i = k - j;
			if (i >= 0 && i < BOARD_SIZE && BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					for (int m = 0; m < 5; ++m) {
						winx[m] = k - j;
						winy[m] = j;
						--j;
					}
					return true;
				}
			}
			else {
				count = 0;
			}
		}
	}
	//左上右下
	for (int k = -10; k < 11; ++k) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			int j = k + i;
			if (j >= 0 && j < BOARD_SIZE && BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					for (int m = 0; m < 5; ++m) {
						winx[m] = i;
						winy[m] = k + i;
						--i;
					}
					return true;
				}
			}
			else {
				count = 0;
			}
		}
	}
	return false;
}

enum condition {
	notBlack,
	notForbidden,
	blackwin,
	doubleThree,
	doubleFour,
	longConnection
};
//黑棋禁手

//condition KeyPointForbiddenCheck(int x, int y, int adjsame, int direction);

condition ForbiddenCheck(int x,int y) {
	if (step % 2 == 1) return notBlack;//白棋不禁手

	//数组下标表示方向
	/*
	左上7 向上0 右上1
	向左6       向右2
	左下5 向下4 右下3
	*/
	int adjsame[8] = { 0 };//与(x,y)相邻连续黑棋数 adjacent
	int adjempty[8] = { 0 };//adjsame后相邻连续空位数
	int jumpsame[8] = { 0 };//adjempty后连续黑棋数
	int jumpempty[8] = { 0 };//jumpsame后连续空位数
	int jumpjumpsame[8] = { 0 };//jumpempty后连续黑棋数
	
	int _x = x, _y = y;

	//向上
	for (_y = y - 1; (_y >= 0) && (BOARD[step][x][_y] == blackc); _y--, adjsame[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == empty; _y--, adjempty[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == blackc; _y--, jumpsame[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == empty; _y--, jumpempty[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == blackc; _y--, jumpjumpsame[0]++);

	//右上
	for (_x = x + 1,_y = y - 1; (_x < 15)&&(_y >= 0) && (BOARD[step][_x][_y] == blackc);_x++, _y--, adjsame[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x++, _y--, adjempty[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x++, _y--, jumpsame[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x++, _y--, jumpempty[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x++, _y--, jumpjumpsame[1]++);

	//向右
	for (_x = x + 1; (_x < 15) && (BOARD[step][_x][y] == blackc); _x++, adjsame[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == empty; _x++, adjempty[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == blackc; _x++, jumpsame[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == empty; _x++, jumpempty[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == blackc; _x++, jumpjumpsame[2]++);

	//右下
	for (_x = x + 1, _y = y + 1; (_x < 15) && (_y < 15) && (BOARD[step][_x][_y] == blackc); _x++, _y++, adjsame[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == empty; _x++, _y++, adjempty[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x++, _y++, jumpsame[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == empty; _x++, _y++, jumpempty[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x++, _y++, jumpjumpsame[3]++);

	//向下
	for (_y = y + 1; (_y < 15) && (BOARD[step][x][_y] == blackc); _y++, adjsame[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == empty; _y++, adjempty[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == blackc; _y++, jumpsame[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == empty; _y++, jumpempty[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == blackc; _y++, jumpjumpsame[4]++);

	//左下
	for (_x = x - 1, _y = y + 1; (_x >= 0) && (_y < 15) && (BOARD[step][_x][_y] == blackc); _x--, _y++, adjsame[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == empty; _x--, _y++, adjempty[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x--, _y++, jumpsame[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == empty; _x--, _y++, jumpempty[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x--, _y++, jumpjumpsame[5]++);

	//向左
	for (_x = x - 1; (_x >= 0) && (BOARD[step][_x][y] == blackc); _x--, adjsame[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == empty; _x--, adjempty[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == blackc; _x--, jumpsame[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == empty; _x--, jumpempty[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == blackc; _x--, jumpjumpsame[6]++);

	//左上
	for (_x = x - 1, _y = y - 1; (_x >= 0) && (_y >= 0) && (BOARD[step][_x][_y] == blackc); _x--, _y--, adjsame[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x--, _y--, adjempty[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x--, _y--, jumpsame[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x--, _y--, jumpempty[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x--, _y--, jumpjumpsame[7]++);

	//先检查是否成连五，若成连五，优先于禁手，黑棋获胜
	/*
	八个方向
	左上7 向上0 右上1
	向左6       向右2
	左下5 向下4 右下3

	四条线
	0   竖直   ↑↓
	1 左下右上 ↙↗
	2   水平   ←→
	3 左上右下 ↖↘
	*/
	for (int i = 0; i < 4; ++i) {
		if (adjsame[i] + adjsame[i + 4] == 4) {
			winx[0] = x;
			winy[0] = y;
			int count = 1;
			int m = adjsame[i];
			int n = adjsame[i + 4];
			switch (i) {
			case 0:
				for (; m > 0; --m) {
					winy[count] = winy[0] - (adjsame[i] - m + 1);
					winx[count] = winx[0];
					count++;
				}
				for (; n > 0; --n) {
					winy[count] = winy[0] + (adjsame[i + 4] - n + 1);
					winx[count] = winx[0];
					count++;
				}
				break;
			case 1:
				for (; m > 0; --m) {
					winy[count] = winy[0] - (adjsame[i] - m + 1);
					winx[count] = winx[0] + (adjsame[i] - m + 1);
					count++;
				}
				for (; n > 0; --n) {
					winy[count] = winy[0] + (adjsame[i + 4] - n + 1);
					winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
					count++;
				}
				break;
			case 2:
				for (; m > 0; --m) {
					winy[count] = winy[0];
					winx[count] = winx[0] + (adjsame[i] - m + 1);
					count++;
				}
				for (; n > 0; --n) {
					winy[count] = winy[0];
					winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
					count++;
				}
				break;
			case 3:
				for (; m > 0; --m) {
					winy[count] = winy[0] + (adjsame[i] - m + 1);
					winx[count] = winx[0] + (adjsame[i] - m + 1);
					count++;
				}
				for (; n > 0; --n) {
					winy[count] = winy[0] - (adjsame[i + 4] - n + 1);
					winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
					count++;
				}
				break;
			default: break;
			}
			return blackwin;
		}
	}

	//————禁手判断————
	int three = 0, four = 0;//活三数、活四+冲四数


	for (int i = 0; i < 4; ++i) {
		//长连禁手
		if (adjsame[i] + adjsame[i + 4] >= 5) return longConnection;//五子以上相连

		//四子相连：?●●●●?
		// ?指除黑棋外的所有
		else if (adjsame[i] + adjsame[i + 4] == 3) {

			//活四、冲四判断
			bool isFour = false;

			//?空●●●●?
			if (adjempty[i] > 0) { 
				//通过递归判断关键点是否可下
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				isFour = true;
			}

			//?●●●●空?
			if (adjempty[i + 4] > 0) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				isFour = true;
			}
			if (isFour) four++;
		}

		//三子相连：?●●●?
		else if (adjsame[i] + adjsame[i + 4] == 2) {

			//活四、冲四判断
			
			//?●空●●●?
			if (adjempty[i] == 1 && jumpsame[i] == 1) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}
			//?●●●空●?
			if (adjempty[i+4] == 1 && jumpsame[i+4] == 1) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//活三判断
			bool isThree = false;

			//?空空●●●空？
			if (((adjempty[i] > 2) || ((adjempty[i] == 2) && (jumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				isThree = true;
			}

			//?空●●●空空？
			if (((adjempty[i + 4] > 2) || ((adjempty[i + 4] == 2) && (jumpsame[i] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				isThree = true;
			}

			if(isThree) three++;
		}

		//二子相连 ?●●?
		else if (adjsame[i] + adjsame[i + 4] == 1) {

			//活四、冲四判断

			//?●●空●●?
			if (adjempty[i] == 1 && jumpsame[i] == 2) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}
			if (adjempty[i+4] == 1 && jumpsame[i+4] == 2) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//活三判断

			//?空●空●●空?
			if ((adjempty[i] == 1) && (adjsame[i+4] == 1) && ((jumpempty[i+4] > 1) || ((jumpempty[i+4] == 1) && (jumpjumpsame[i+4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				three++;
			}
			if ((adjempty[i+4] == 1) && (adjsame[i] == 1) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				three++;
			}
		}

		//一子 ?●?
		else if (adjsame[i] + adjsame[i + 4] == 0) {

			//活四、冲四判断

			//?●●●空●？
			if (adjempty[i] == 1 && jumpsame[i] == 3) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}

			//?●空●●●?
			if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 3) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//活三判断

			//?空●●空●空？
			if ((adjempty[i] == 1) && (jumpsame[i] == 2) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				three++;
			}

			//?空●空●●空?
			if ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 2) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i + 4], i + 4) == notForbidden) 
				three++;
			}
		}
	}
	//————禁手判断结束————

	if (four > 1) return doubleFour;
	if (three > 1)return doubleThree;
	return notForbidden;
}

/*condition KeyPointForbiddenCheck(int x, int y, int adjsame, int direction) {
	int i = 0, j = 0;
	adjsame++;
	if (direction >= 4)adjsame = -adjsame;

	//计算关键点坐标
	switch (direction % 4) {
	case 0:
		i = x;
		j = y - adjsame;
		break;
	case 1:
		i = x + adjsame;
		j = y - adjsame;
		break;
	case 2:
		i = x + adjsame;
		j = y;
		break;
	case 3:
		i = x + adjsame;
		j = y + adjsame;
	default:break;
	}

	//向棋盘中放入棋子
	BOARD[step][x][y] = blackc;
	BOARD[step][x][y] = blackc;

	//检查关键点
	condition result = ForbiddenCheck(i, j);

	//还原
	BOARD[step][x][y] = empty;
	BOARD[step][x][y] = empty;

	return result;
}*/

bool computer = false;//是否人机对战
bool XianShou = true;//是否先手

class Board {
private:
	std::vector<std::vector<state>>board;
public:
	Board() :board(BOARD_SIZE, std::vector<state>(BOARD_SIZE, state::empty)) {}

	void cp(){
		for (int p = 0; p < BOARD_SIZE; ++p) {
			for (int q = 0; q < BOARD_SIZE; ++q) {
				board[p][q] = BOARD[step][p][q];
			}
		}
		return;
	}
	//可直接复制之前的
	condition ForbiddenCheck(int x, int y) {
		if (step % 2 == 1) return notBlack;//白棋不禁手

		//数组下标表示方向
		/*
		左上7 向上0 右上1
		向左6       向右2
		左下5 向下4 右下3
		*/
		int adjsame[8] = { 0 };//与(x,y)相邻连续黑棋数 adjacent
		int adjempty[8] = { 0 };//adjsame后相邻连续空位数
		int jumpsame[8] = { 0 };//adjempty后连续黑棋数
		int jumpempty[8] = { 0 };//jumpsame后连续空位数
		int jumpjumpsame[8] = { 0 };//jumpempty后连续黑棋数

		int _x = x, _y = y;

		//向上
		for (_y = y - 1; (_y >= 0) && (board[x][_y] == blackc); _y--, adjsame[0]++);
		for (; (_y >= 0) && board[x][_y] == empty; _y--, adjempty[0]++);
		for (; (_y >= 0) && board[x][_y] == blackc; _y--, jumpsame[0]++);
		for (; (_y >= 0) && board[x][_y] == empty; _y--, jumpempty[0]++);
		for (; (_y >= 0) && board[x][_y] == blackc; _y--, jumpjumpsame[0]++);

		//右上
		for (_x = x + 1, _y = y - 1; (_x < 15) && (_y >= 0) && (board[_x][_y] == blackc); _x++, _y--, adjsame[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == empty; _x++, _y--, adjempty[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == blackc; _x++, _y--, jumpsame[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == empty; _x++, _y--, jumpempty[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == blackc; _x++, _y--, jumpjumpsame[1]++);

		//向右
		for (_x = x + 1; (_x < 15) && (board[_x][y] == blackc); _x++, adjsame[2]++);
		for (; (_x < 15) && board[_x][y] == empty; _x++, adjempty[2]++);
		for (; (_x < 15) && board[_x][y] == blackc; _x++, jumpsame[2]++);
		for (; (_x < 15) && board[_x][y] == empty; _x++, jumpempty[2]++);
		for (; (_x < 15) && board[_x][y] == blackc; _x++, jumpjumpsame[2]++);

		//右下
		for (_x = x + 1, _y = y + 1; (_x < 15) && (_y < 15) && (board[_x][_y] == blackc); _x++, _y++, adjsame[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == empty; _x++, _y++, adjempty[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == blackc; _x++, _y++, jumpsame[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == empty; _x++, _y++, jumpempty[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == blackc; _x++, _y++, jumpjumpsame[3]++);

		//向下
		for (_y = y + 1; (_y < 15) && (board[x][_y] == blackc); _y++, adjsame[4]++);
		for (; (_y < 15) && board[x][_y] == empty; _y++, adjempty[4]++);
		for (; (_y < 15) && board[x][_y] == blackc; _y++, jumpsame[4]++);
		for (; (_y < 15) && board[x][_y] == empty; _y++, jumpempty[4]++);
		for (; (_y < 15) && board[x][_y] == blackc; _y++, jumpjumpsame[4]++);

		//左下
		for (_x = x - 1, _y = y + 1; (_x >= 0) && (_y < 15) && (board[_x][_y] == blackc); _x--, _y++, adjsame[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == empty; _x--, _y++, adjempty[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == blackc; _x--, _y++, jumpsame[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == empty; _x--, _y++, jumpempty[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == blackc; _x--, _y++, jumpjumpsame[5]++);

		//向左
		for (_x = x - 1; (_x >= 0) && (board[_x][y] == blackc); _x--, adjsame[6]++);
		for (; (_x >= 0) && board[_x][y] == empty; _x--, adjempty[6]++);
		for (; (_x >= 0) && board[_x][y] == blackc; _x--, jumpsame[6]++);
		for (; (_x >= 0) && board[_x][y] == empty; _x--, jumpempty[6]++);
		for (; (_x >= 0) && board[_x][y] == blackc; _x--, jumpjumpsame[6]++);

		//左上
		for (_x = x - 1, _y = y - 1; (_x >= 0) && (_y >= 0) && (board[_x][_y] == blackc); _x--, _y--, adjsame[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == empty; _x--, _y--, adjempty[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == blackc; _x--, _y--, jumpsame[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == empty; _x--, _y--, jumpempty[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == blackc; _x--, _y--, jumpjumpsame[7]++);

		//先检查是否成连五，若成连五，优先于禁手，黑棋获胜
		/*
		八个方向
		左上7 向上0 右上1
		向左6       向右2
		左下5 向下4 右下3

		四条线
		0   竖直   ↑↓
		1 左下右上 ↙↗
		2   水平   ←→
		3 左上右下 ↖↘
		*/
		for (int i = 0; i < 4; ++i) {
			if (adjsame[i] + adjsame[i + 4] == 4) {
				/*
				winx[0] = x;
				winy[0] = y;
				int count = 1;
				int m = adjsame[i];
				int n = adjsame[i + 4];
				switch (i) {
				case 0:
					for (; m > 0; --m) {
						winy[count] = winy[0] - (adjsame[i] - m + 1);
						winx[count] = winx[0];
						count++;
					}
					for (; n > 0; --n) {
						winy[count] = winy[0] + (adjsame[i + 4] - n + 1);
						winx[count] = winx[0];
						count++;
					}
					break;
				case 1:
					for (; m > 0; --m) {
						winy[count] = winy[0] - (adjsame[i] - m + 1);
						winx[count] = winx[0] + (adjsame[i] - m + 1);
						count++;
					}
					for (; n > 0; --n) {
						winy[count] = winy[0] + (adjsame[i + 4] - n + 1);
						winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
						count++;
					}
					break;
				case 2:
					for (; m > 0; --m) {
						winy[count] = winy[0];
						winx[count] = winx[0] + (adjsame[i] - m + 1);
						count++;
					}
					for (; n > 0; --n) {
						winy[count] = winy[0];
						winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
						count++;
					}
					break;
				case 3:
					for (; m > 0; --m) {
						winy[count] = winy[0] + (adjsame[i] - m + 1);
						winx[count] = winx[0] + (adjsame[i] - m + 1);
						count++;
					}
					for (; n > 0; --n) {
						winy[count] = winy[0] - (adjsame[i + 4] - n + 1);
						winx[count] = winx[0] - (adjsame[i + 4] - n + 1);
						count++;
					}
					break;
				default: break;
				}
				*/
				return blackwin;
			}
		}

		//————禁手判断————
		int three = 0, four = 0;//活三数、活四+冲四数


		for (int i = 0; i < 4; ++i) {
			//长连禁手
			if (adjsame[i] + adjsame[i + 4] >= 5) return longConnection;//五子以上相连

			//四子相连：?●●●●?
			// ?指除黑棋外的所有
			else if (adjsame[i] + adjsame[i + 4] == 3) {

				//活四、冲四判断
				bool isFour = false;

				//?空●●●●?
				if (adjempty[i] > 0) {
					//通过递归判断关键点是否可下
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					isFour = true;
				}

				//?●●●●空?
				if (adjempty[i + 4] > 0) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					isFour = true;
				}
				if (isFour) four++;
			}

			//三子相连：?●●●?
			else if (adjsame[i] + adjsame[i + 4] == 2) {

				//活四、冲四判断

				//?●空●●●?
				if (adjempty[i] == 1 && jumpsame[i] == 1) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}
				//?●●●空●?
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 1) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//活三判断
				bool isThree = false;

				//?空空●●●空？
				if (((adjempty[i] > 2) || ((adjempty[i] == 2) && (jumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					isThree = true;
				}

				//?空●●●空空？
				if (((adjempty[i + 4] > 2) || ((adjempty[i + 4] == 2) && (jumpsame[i] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					isThree = true;
				}

				if (isThree) three++;
			}

			//二子相连 ?●●?
			else if (adjsame[i] + adjsame[i + 4] == 1) {

				//活四、冲四判断

				//?●●空●●?
				if (adjempty[i] == 1 && jumpsame[i] == 2) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 2) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//活三判断

				//?空●空●●空?
				if ((adjempty[i] == 1) && (adjsame[i + 4] == 1) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					three++;
				}
				if ((adjempty[i + 4] == 1) && (adjsame[i] == 1) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					three++;
				}
			}

			//一子 ?●?
			else if (adjsame[i] + adjsame[i + 4] == 0) {

				//活四、冲四判断

				//?●●●空●？
				if (adjempty[i] == 1 && jumpsame[i] == 3) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}

				//?●空●●●?
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 3) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//活三判断

				//?空●●空●空？
				if ((adjempty[i] == 1) && (jumpsame[i] == 2) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					three++;
				}

				//?空●空●●空?
				if ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 2) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i + 4], i + 4) == notForbidden) 
					three++;
				}
			}
		}
		//————禁手判断结束————

		if (four > 1) return doubleFour;
		if (three > 1)return doubleThree;
		return notForbidden;
	}
	// 落子
	bool placePiece(int row, int col, state player) {
		if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != state::empty||((player==blackc)&&((ForbiddenCheck(row, col) != notForbidden)|| (ForbiddenCheck(row, col) != blackwin)))) {
			return false;
		}
		board[row][col] = player;
		return true;
	}
	// 获取棋盘状态
	state getState(int row, int col) const {
		return board[row][col];
	}
	//可直接复制之前的
	bool Won(state color) {
		//横向
		for (int i = 0; i < BOARD_SIZE; ++i) {
			int count = 0;
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (board[i][j] == color) {
					count++;
					if (count == 5) {
						return true;
					}
				}
				else {
					count = 0;
				}
			}
		}
		//纵向
		for (int j = 0; j < BOARD_SIZE; ++j) {
			int count = 0;
			for (int i = 0; i < BOARD_SIZE; ++i) {
				if (board[i][j] == color) {
					count++;
					if (count == 5) {
						return true;
					}
				}
				else {
					count = 0;
				}
			}
		}
		//左下右上
		for (int k = 4; k < 25; ++k) {
			int count = 0;
			for (int j = 0; j < BOARD_SIZE; ++j) {
				int i = k - j;
				if (i >= 0 && i < BOARD_SIZE && board[i][j] == color) {
					count++;
					if (count == 5) {
						return true;
					}
				}
				else {
					count = 0;
				}
			}
		}
		//左上右下
		for (int k = -10; k < 11; ++k) {
			int count = 0;
			for (int i = 0; i < BOARD_SIZE; ++i) {
				int j = k + i;
				if (j >= 0 && j < BOARD_SIZE && board[i][j] == color) {
					count++;
					if (count == 5) {
						return true;
					}
				}
				else {
					count = 0;
				}
			}
		}
		return false;
	}

	// 检查棋盘是否已满
	bool isFull() const {
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (board[i][j] == empty) {
					return false;
				}
			}
		}
		return true;
	}
};
/*
//蒙特卡洛
int M(Board board,state player) {
	bool turn = 0;//0 black，1 white
	if (player==blackc) {
		turn = 0;
	}
	else if(player == whitec){
		turn = 1;
	}
	std::vector<std::vector<int>> emptyCells;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (BOARD[step][i][j] == empty) {
				emptyCells.emplace_back(std::vector<int>{i, j});
			}
		}
	}
	std::random_device rd;
	std::mt19937 gen(rd());//伪随机数生成器类
	std::uniform_int_distribution<> dis(0, emptyCells.size() - 1);//指定范围内随机非负数

	int winCount = 0;
	int Simulations = 1000;
	for (int i = 0; i < Simulations; ++i) {
		Board tempBoard = board;
		state currentplayer = player;
		bool currentturn = turn;
		while (true) {
			int index = dis(gen);
			int row = emptyCells[index][0];
			int col = emptyCells[index][1];
			if (tempBoard.placePiece(row, col, player)) {
				if (tempBoard.Win(turn)) {
					if (currentplayer == player) {
						++winCount;
					}
					break;
				}
				currentplayer = (currentplayer == state::blackc) ? state::whitec : state::blackc;
				turn = (currentplayer == state::blackc) ? 0 : 1;
			}
		}
	}
	return winCount;
}

std::vector<int> ComputerMove (Board board, state player) {
	int BestScore = -1;
	std::vector<int> bestMove;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (board.getState(i, j) == state::empty) {
				Board tempBoard = board;
				tempBoard.placePiece(i, j, player);
				int score = M(tempBoard, player);
				if (score > BestScore) {
					BestScore = score;
					bestMove = { i,j };
				}
			}
		}
	}
	return bestMove;
}
*/

int evaluate(Board board, state color) {
	/*
	只判断胜利还是太弱了
	if (board.Won(color)) {
		return 1000;
	}
	else if (board.Won(color == blackc ? whitec : blackc)) {
		return -1000;
	}
	return 0;
	*/

	int computerscore = 0;
	int peoplescore = 0;

	//横向
	//电脑
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					return 1000;
				}
			}
			else {
				computerscore += count * 10;
				count = 0;
			}
		}
	}
	//人
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (BOARD[step][i][j] == (color == blackc ? whitec : blackc)) {
				count++;
				if (count == 5) {
					return -1000;
				}
			}
			else {
				peoplescore += count * 10;
				count = 0;
			}
		}
	}

	//纵向
	//电脑
	for (int j = 0; j < BOARD_SIZE; ++j) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			if (BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					return 1000;
				}
			}
			else {
				computerscore += count * 10;
				count = 0;
			}
		}
	}
	//人
	for (int j = 0; j < BOARD_SIZE; ++j) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			if (BOARD[step][i][j] == (color == blackc ? whitec : blackc)) {
				count++;
				if (count == 5) {
					return -1000;
				}
			}
			else {
				peoplescore += count * 10;
				count = 0;
			}
		}
	}

	//左下右上
	//电脑
	for (int k = 4; k < 25; ++k) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			int i = k - j;
			if (i >= 0 && i < BOARD_SIZE && BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					return 1000;
				}
			}
			else {
				computerscore += count * 10;
				count = 0;
			}
		}
	}
	//人
	for (int k = 4; k < 25; ++k) {
		int count = 0;
		for (int j = 0; j < BOARD_SIZE; ++j) {
			int i = k - j;
			if (i >= 0 && i < BOARD_SIZE && BOARD[step][i][j] == (color == blackc ? whitec : blackc)) {
				count++;
				if (count == 5) {
					return -1000;
				}
			}
			else {
				peoplescore += count * 10;
				count = 0;
			}
		}
	}

	//左上右下
	//电脑
	for (int k = -10; k < 11; ++k) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			int j = k + i;
			if (j >= 0 && j < BOARD_SIZE && BOARD[step][i][j] == color) {
				count++;
				if (count == 5) {
					return 1000;
				}
			}
			else {
				computerscore += count * 10;
				count = 0;
			}
		}
	}
	//人
	for (int k = -10; k < 11; ++k) {
		int count = 0;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			int j = k + i;
			if (j >= 0 && j < BOARD_SIZE && BOARD[step][i][j] == (color == blackc ? whitec : blackc)) {
				count++;
				if (count == 5) {
					return -1000;
				}
			}
			else {
				peoplescore += count * 10;
				count = 0;
			}
		}
	}

	return computerscore-peoplescore;
}

// alpha-beta 剪枝的极小极大算法
int minimax(Board board, int depth, int alpha, int beta, bool isMaximizing, state color) {
	if (depth == 0 || board.Won(color) || board.Won(color == blackc ? whitec : blackc) || board.isFull()) {
		return evaluate(board, color);
	}

	if (isMaximizing) {
		int maxEval = -10000;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (board.getState(i, j) == empty) {
					board.placePiece(i, j, color);
					int eval = minimax(board, depth - 1, alpha, beta, false, color);
					board.placePiece(i, j, empty);
					maxEval = max(maxEval, eval);
					alpha = max(alpha, eval);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		return maxEval;
	}
	else {
		int minEval = 10000;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (board.getState(i, j) == empty) {
					board.placePiece(i, j, color == blackc ? whitec : blackc);
					int eval = minimax(board, depth - 1, alpha, beta, true, color);
					board.placePiece(i, j, empty);
					minEval = min(minEval, eval);
					beta = min(beta, eval);
					if (beta <= alpha) {
						break;
					}
				}
			}
		}
		return minEval;
	}
}

std::pair<int, int> aiMove(Board board, state color) {
	int bestEval = -10000;
	std::pair<int, int> bestMove(-1, -1);
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (board.getState(i, j) == empty) {
				board.placePiece(i, j, color);
				int eval = minimax(board, 2, -10000, 10000, false, color);
				board.placePiece(i, j, empty);
				if (eval > bestEval) {
					bestEval = eval;
					bestMove = { i, j };
				}
			}
		}
	}
	return bestMove;
}

//保存
void SAVE() {
	std::ofstream outFile("memory.txt");
	if (!outFile.is_open()) {
		std::cerr << "无法打开文件存储棋局！" << std::endl;
		return;
	}
	outFile << step << std::endl;
	for (int k = 0; k <= step; ++k) {
		outFile << placex[k] << std::endl;
		outFile << placey[k] << std::endl;
	}
	for (int k = 0; k <= step; ++k) {
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (BOARD[k][i][j] == empty) {
					outFile << 0 << " ";
				}
				else if (BOARD[k][i][j] == blackc) {
					outFile << 1 << " ";
				}
				else if (BOARD[k][i][j] == whitec) {
					outFile << 2 << " ";
				}

			}
			outFile << std::endl;
		}
		outFile << std::endl << std::endl;
	}

	outFile.close();

	std::cout << "已成功存储棋局~" << std::endl;
	return;
}

//读取
void READ() {
	std::ifstream inFile("memory.txt");
	if (!inFile.is_open()) {
		std::cerr << "无法打开文件读取棋局！" << std::endl;
		return;
	}
	inFile >> step;
	std::cout << step << std::endl;
	for (int k = 0; k <= step; ++k) {
		inFile >> placex[k];
		inFile >> placey[k];
	}
	int num;
	for (int k = 0; k <= step; ++k) {
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				inFile >> num;
				if (num == 0) {
					BOARD[k][i][j] = empty;
				}
				else if (num == 1) {
					BOARD[k][i][j] = blackc;
				}
				else if (num == 2) {
					BOARD[k][i][j] = whitec;
				}

			}
		}
	}
	inFile.close();

	std::cout << "已成功读取棋局~" << std::endl;
	return;
}

//透明图片
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg) {
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER,0,255,AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

//按钮（没写成类是因为期间出了点不会解决的问题）
struct Button {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	IMAGE image;
	IMAGE image_change;
	bool isHover = false;
	bool isClicked = false;
};
//初始化
void inb(Button& button,int _x, int _y, int _width, int _height) {
	button.x = _x;
	button.y = _y;
	button.width = _width;
	button.height = _height;
}//initialize button
//绘制
void drawbutton(Button button) {
	if (button.isHover) {
		putimage(button.x, button.y, &button.image_change);
	}
	else {
		putimage(button.x, button.y, &button.image);
	}
}
//绘制透明背景
void transdrawbutton(Button button) {
	if (button.isHover) {
		transparentimage(NULL, button.x, button.y, &button.image_change);
	}
	else {
		transparentimage(NULL, button.x, button.y, &button.image);
	}
}
//鼠标是否在按钮上
void checkHover(Button* button,int mouseX, int mouseY) {
	button->isHover = ((mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= (button->y)) && (mouseY <= (button->y) + (button->height)));
}
//电子皮肤的图标需要进行一点偏移
void JX_checkHover(Button* button, int mouseX, int mouseY,int dy) {
	button->isHover = ((mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= ((button->y)+dy)) && (mouseY <= (button->y) + (button->height)));
}
//是否点击按钮
void checkClick(Button* button,int mouseX, int mouseY) {
	button->isClicked = ((button->isHover) && (mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= (button->y)) && (mouseY <= (button->y) + (button->height)));
}

//飞天皮肤主页面
class FeiTianStartPage {
private:
	IMAGE spb;
	Button start, setting, rule;
	IMAGE st, stc, se, sec, ru, ruc;
	double lx = 910, ly = 530, rx = 0 - 356 - 10, ry = 570;
	double lt = 0, rt = 0;
	int time = 0, add = 0;
	bool lanother_state = false, ranother_state = false;
	IMAGE fl, fr;
public:
	FeiTianStartPage()
		{
		loadimage(&spb, "resources/start_page_background.png");
		loadimage(&st, "resources/start.png");
		loadimage(&stc, "resources/start_change.png");
		loadimage(&se, "resources/setting.png");
		loadimage(&sec, "resources/setting_change.png");
		loadimage(&ru, "resources/rule.png");
		loadimage(&ruc, "resources/rule_change.png");
		loadimage(&fl, "resources/feitian_left.png");
		loadimage(&fr, "resources/feitian_right.png");
		inb(rule, 333, 301, 234, 98);
		inb(start, 333, 406, 234, 98);
		inb(setting, 333, 511, 234, 98);
		rule.image = ru;
		rule.image_change = ruc;
		start.image = st;
		start.image_change = stc;
		setting.image = se;
		setting.image_change = sec;
	};
	~FeiTianStartPage() {};
	void draw() {
		putimage(0, 0, &spb);
		drawbutton(rule);
		drawbutton(start);
		drawbutton(setting);
		transparentimage(NULL, lx, ly, &fl);
		transparentimage(NULL, rx, ry, &fr);
	}
	void update() {
		//左图动作
		if (lanother_state) {
			time++;
			time %= 50;//防止太快
			if (time == 0) {
				add+=5;
				if (add > 628) {
					add &= 628;
				}
				lx = 10 + 5 * sin(add);
				ly = 620 + 5 * cos(add);
			}
		}
		else {
			time++;//入场动作
			time %= 8;
			if (lx <= 11) {
				lanother_state = true;
			}
			else if (time == 0) {
				lx -= 10;
				ly += 1;
			}
		}
		//右图动作
		if (ranother_state) {
			if (time == 0) {
				add += 5;
				rx = 534 - 5 * sin(add);
				ry = 660 + 5 * cos(add);
			}
		}
		else {//入场动作
			if (rx >= 900-11-356) {
				ranother_state = true;
			}
			else if(time == 0) {
				rx += 10;
				ry += 1;
			}
		}
	}
	bool shouldGo() {
		//入场动作是否结束
		return (lanother_state && ranother_state);
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		checkHover(&start, msg.x, msg.y);
		checkHover(&setting, msg.x, msg.y);
		checkHover(&rule, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&start, msg.x, msg.y);
			checkClick(&setting, msg.x, msg.y);
			checkClick(&rule, msg.x, msg.y);
			if (start.isClicked) {
				std::cout << "start clicked" << std::endl;
				start.isHover = false;
				PAGE = ftready;//点击开始进入准备界面
			}
			else if (setting.isClicked) {
				std::cout << "setting clicked" << std::endl;
				setting.isHover = false;
				PAGE = ftSetting;//点击设置进入换肤界面
			}
			else if (rule.isClicked) {
				std::cout << "rule clicked" << std::endl;
				rule.isHover = false;
				PAGE = ftRule;//点击规则进入规则介绍
			}
			else std::cout << "nothing happend" << std::endl;
			break;
		}
	}
};
//飞天皮肤准备进入页面
class FeiTianReady {
private:
	IMAGE blur, lb;
	IMAGE n, nc, s, sc;
	Button newstart, save;
	int pagestep = 1;//标示页数
	IMAGE vp, vpc, vc, vcc;
	Button pvp, pvc;
	IMAGE f, fc, l, lc;
	Button first, last;
public:
	FeiTianReady() {
		loadimage(&blur, "resources/blur.png");
		loadimage(&lb, "resources/little_background.png");
		loadimage(&n, "resources/new.png");
		loadimage(&nc, "resources/new_change.png");
		loadimage(&s, "resources/read.png");
		loadimage(&sc, "resources/read_change.png");
		loadimage(&vp, "resources/pvp.png");
		loadimage(&vpc, "resources/pvp_change.png");
		loadimage(&vc, "resources/pvc.png");
		loadimage(&vcc, "resources/pvc_change.png");
		loadimage(&f, "resources/xs.png");
		loadimage(&fc, "resources/xs_change.png");
		loadimage(&l, "resources/hs.png");
		loadimage(&lc, "resources/hs_change.png");
		inb(newstart, 333, 350, 234, 98);
		inb(save, 333, 452, 234, 98);
		inb(pvp, 180, 180, 234, 98);
		inb(pvc, 486, 180, 234, 98);
		inb(first, 180, 622, 234, 98);
		inb(last, 486, 622, 234, 98);
		newstart.image = n;
		newstart.image_change = nc;
		save.image = s;
		save.image_change = sc;
		pvp.image = vp;
		pvp.image_change = vpc;
		pvc.image = vc;
		pvc.image_change = vcc;
		first.image = f;
		first.image_change = fc;
		last.image = l;
		last.image_change = lc;
	}
	~FeiTianReady() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		drawbutton(newstart);
		drawbutton(save);
		if (pagestep == 2) {//第二页，选模式
			transparentimage(NULL, 0, 0, &blur);
			drawbutton(pvp);
			drawbutton(pvc);
		}
		if (pagestep == 3) {//第三页，人机对战下选先后手
			transparentimage(NULL, 0, 0, &blur);
			drawbutton(pvp);
			drawbutton(pvc);
			transparentimage(NULL, 0, 0, &blur);
			drawbutton(first);
			drawbutton(last);
		}
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		if (pagestep == 1) {
			checkHover(&newstart, msg.x, msg.y);
			checkHover(&save, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&newstart, msg.x, msg.y);
				checkClick(&save, msg.x, msg.y);
				if (newstart.isClicked) {
					step = 0;//新的开始
					pagestep = 2;//下一页
					newstart.isHover = false;
					std::cout << "newstart clicked" << std::endl;
				}
				else if (save.isClicked) {
					READ();//读取存档
					pagestep = 2;
					save.isHover = false;
					std::cout << "read clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					std::cout << "return" << std::endl;
					PAGE = ftStart;//返回主页面
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}
		else if (pagestep == 2) {
			checkHover(&pvp, msg.x, msg.y);
			checkHover(&pvc, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&pvp, msg.x, msg.y);
				checkClick(&pvc, msg.x, msg.y);
				if (pvp.isClicked) {
					computer = false;//不运行人工智能
					PAGE = ftGame;
					pagestep = 1;//开始游戏后page初始化
					pvp.isHover = false;
					std::cout << "pvp clicked" << std::endl;
				}
				else if (pvc.isClicked) {
					computer = true;//运行人工智能
					pagestep = 3;//下一页
					pvc.isHover = false;
					std::cout << "pvc clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep = 1;//返回上一页
					std::cout << "return pagestep1" << std::endl;
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}
		else if (pagestep == 3) {
			checkHover(&first, msg.x, msg.y);
			checkHover(&last, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&first, msg.x, msg.y);
				checkClick(&last, msg.x, msg.y);
				if (first.isClicked) {
					XianShou = true;//先手
					PAGE = ftGame;
					pagestep = 1;//初始化
					first.isHover = false;
					std::cout << "first clicked" << std::endl;
				}
				else if (last.isClicked) {
					XianShou = false;//后手
					PAGE = ftGame;
					pagestep = 1;//初始化
					pvc.isHover = false;
					std::cout << "last clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep = 2;//返回上一页
					std::cout << "return pagestep2" << std::endl;
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}

	}
};
//飞天皮肤规则页面
class FeiTianRule {
private:
	IMAGE blur;
	IMAGE r1, r2, r3, r4;
	IMAGE n, nc, p, pc;
	Button next, previous;
	int page = 1;//页数
public:
	FeiTianRule() {
		loadimage(&blur, "resources/blur.png");
		loadimage(&r1, "resources/rule1.png");
		loadimage(&r2, "resources/rule2.png");
		loadimage(&r3, "resources/rule3.png");
		loadimage(&r4, "resources/rule4.png");
		loadimage(&n, "resources/next.png");
		loadimage(&nc, "resources/next_change.png");
		loadimage(&p, "resources/previous.png");
		loadimage(&pc, "resources/previous_change.png");
		inb(previous, 158, 652, 214, 90);
		inb(next, 528, 652, 214, 90);
		next.image = n;
		next.image_change = nc;
		previous.image = p;
		previous.image_change = pc;
	}
	~FeiTianRule() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		//不同页显示不同按钮
		if (page == 1) {
			putimage(150, 150, &r1);
			drawbutton(next);
		}
		else if (page == 2) {
			putimage(150, 150, &r2);
			drawbutton(next);
			drawbutton(previous);
		}
		else if (page == 3) {
			putimage(150, 150, &r3);
			drawbutton(next);
			drawbutton(previous);
		}
		else if (page == 4) {
			putimage(150, 150, &r4);
			drawbutton(previous);
		}
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		//不同页检测不同按钮
		if (page == 1) {
			checkHover(&next, msg.x, msg.y);
		}
		else if ((page == 2)|| (page == 3)) {
			checkHover(&next, msg.x, msg.y);
			checkHover(&previous, msg.x, msg.y);
		}
		else if (page == 4) {
			checkHover(&previous, msg.x, msg.y);
		}
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&next, msg.x, msg.y);
			checkClick(&previous, msg.x, msg.y);
			if (next.isClicked) {
				std::cout << "next clicked" << std::endl;
				page++;//下一页
				previous.isHover = false;
				next.isHover = false;
			}
			else if (previous.isClicked) {
				std::cout << "previous clicked" << std::endl;
				page--;//上一页
				previous.isHover = false;
				next.isHover = false;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return" << std::endl;
				PAGE = ftStart;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}
		//防止页数出范围
		if (page < 1) {
			page = 1;
		}
		if (page > 4) {
			page = 4;
		}
	}
};
//飞天皮肤设置（换肤）界面
class FeiTianSetting {//点击大图就会换皮肤（没想好有什么别的设置内容）
private:
	IMAGE blur, lb;
	//IMAGE c, cc;
	//Button costume;
public:
	FeiTianSetting() {
		loadimage(&blur, "resources/blur.png");
		loadimage(&lb, "resources/hybriground.png");
		//loadimage(&c, "resources/costume.png");
		//loadimage(&cc, "resources/costume_change.png");
		//inb(costume, 307, 282, 286, 120);
		//costume.image = c;
		//costume.image_change = cc;
	}
	~FeiTianSetting() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		//drawbutton(costume);
	}
	void handleMouse() {
		draw();
		MOUSEMSG msg = GetMouseMsg();
		//checkHover(&costume, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			/*checkClick(&costume, msg.x, msg.y);
			if (costume.isClicked) {
				std::cout << "costume clicked" << std::endl;
				PAGE = jxSetting;
			}*/
			if (msg.x >= 150 && msg.x <= 750 && msg.y >= 150 && msg.y <= 750) {
				std::cout << "costume clicked" << std::endl;
				PAGE = jxSetting;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return game" << std::endl;
				PAGE = ftStart;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}
	}
};
//飞天皮肤下棋界面
class FeiTianBoard {
private:
	IMAGE board, black, white;
	bool turn = false;//0 blackc,1 whitec
	bool shouldbreak = false;
	IMAGE r, rc, s, sc;
	Button regret, stop;
	condition con=notForbidden;
	state computercolor = blackc;
	Board cpboard;
public:
	FeiTianBoard() {
		loadimage(&board, "resources/chessboard_black.png");
		loadimage(&black, "resources/black_chess.png");
		loadimage(&white, "resources/white_chess.png");
		loadimage(&r, "resources/regret.png");
		loadimage(&rc, "resources/regret_change.png");
		loadimage(&s, "resources/stop.png");
		loadimage(&sc, "resources/stop_change.png");
		inb(regret, 755, 385, 130, 130);
		inb(stop, 15, 385, 130, 130);
		regret.image = r;
		regret.image_change = rc;
		stop.image = s;
		stop.image_change = sc;
	}
	~FeiTianBoard() {};
	void draw() {
		putimage(0, 0, &board);
		//特殊标记
		if (step > 0) {
			int x = 0;
			int y = 0;
			//如果结束，高亮胜利原因
			if (end) {
				setfillcolor(RGB(139, 0, 18));//北大红！！！
				setlinecolor(RGB(139, 0, 18));
				for (int i = 0; i < 5; ++i) {
					x = 175 + winx[i] * 550 / 14;
					y = 175 + winy[i] * 550 / 14;
					fillcircle(x, y, R + 2);
				}
			}
			//高亮上一步走在了哪里
			x = 175 + placex[step] * 550 / 14;
			y = 175 + placey[step] * 550 / 14;
			setfillcolor(RGB(114, 214, 191));
			setlinecolor(RGB(114, 214, 191));
			fillcircle(x, y, R+1);
		}
		//根据当前棋局绘图
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (BOARD[step][i][j] == blackc) {//blackc 1
					int x = 175 + i * 550 / 14 - R;
					int y = 175 + j * 550 / 14 - R;
					transparentimage(NULL, x, y, &black);
				}
				else if (BOARD[step][i][j] == whitec) {//whitec 2
					int x = 175 + i * 550 / 14 - R;
					int y = 175 + j * 550 / 14 - R;
					transparentimage(NULL, x, y, &white);
				}
			}
		}
		drawbutton(regret);
		drawbutton(stop);
	}
	//下棋
	void update() {
		step++;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				BOARD[step][i][j] = BOARD[step - 1][i][j];
			}
		}
		if (turn == 0) {
			BOARD[step][placex[step]][placey[step]] = blackc;
		}
		else {
			BOARD[step][placex[step]][placey[step]] = whitec;
		}
		//下完棋后判断是否胜利
		if (con == blackwin) {
			end = true;
		}
		else if (Win(turn)) {
			end = true;
		}
		else {
			turn = step % 2;
		}
	}
	void handleMouse() {
		turn = step % 2;
		MOUSEMSG msg = GetMouseMsg();
		//光标挪动到可下子位置会显示轮廓
		if (!end) {
			for (int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					if (BOARD[step][i][j] == empty) {//empty 0
						int x = 175 + i * 550 / 14;
						int y = 175 + j * 550 / 14;
						if ((msg.x >= x - R) && (msg.x <= x + R) && (msg.y >= y - R) && (msg.y <= y + R)) {
							setfillcolor(BLACK);
							setlinecolor(BLACK);
							circle(x, y, R);
						}
					}
				}
			}
		}
		checkHover(&regret, msg.x, msg.y);
		checkHover(&stop, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&regret, msg.x, msg.y);
			checkClick(&stop, msg.x, msg.y);
			//判断是否点击悔棋
			if (regret.isClicked) {
				std::cout << "regret clicked" << std::endl;
				if (step > 0) {
					if (computer) {
						if (((XianShou) && (computercolor == whitec)) || ((!XianShou) && (computercolor = blackc))) {
							initialize_k(step);
							step--;
							if (step < 0) {
								step = 0;
							}
							initialize_k(step);
							step--;
						}
						
					}
					else {
						initialize_k(step);
						step--;
					}
					if (step < 0) {
						step = 0;
					}
					if (con = blackwin) {
						con = notForbidden;
					}
					end = false;
				}
				regret.isHover = false;
			}
			//判断时候点击暂停
			else if (stop.isClicked) {
				std::cout << "stop clicked" << std::endl;
				PAGE = ftstop;
				stop.isHover = false;
			}
			//非分出胜负时，检索下棋位置
			else if (!end) {
				for (int i = 0; i < BOARD_SIZE; ++i) {
					for (int j = 0; j < BOARD_SIZE; ++j) {
						if (BOARD[step][i][j] == empty) {//empty 0
							int x = 175 + i * 550 / 14;
							int y = 175 + j * 550 / 14;
							if ((msg.x >= x - R) && (msg.x <= x + R) && (msg.y >= y - R) && (msg.y <= y + R)) {
								if (!turn) {
									con = ForbiddenCheck(i, j);
									if (con == doubleFour) {
										std::cout << "四四禁手" << std::endl;
										MessageBox(NULL, "四四禁手", "禁手提醒", MB_OK);
										break;
									}
									else if (con == doubleThree) {
										std::cout << "三三禁手" << std::endl;
										MessageBox(NULL, "三三禁手", "禁手提醒", MB_OK);
										break;
									}
									else if (con == longConnection) {
										std::cout << "长连禁手" << std::endl;
										MessageBox(NULL, "长连禁手", "禁手提醒", MB_OK);
										break;
									}

								}
								placex[step + 1] = i;
								placey[step + 1] = j;
								shouldbreak = true;
								break;
							}
						}
					}
					if (shouldbreak)break;
				}
				if (shouldbreak) {
					update();
					shouldbreak = false;
				}
			}
			break;
		}
	}
	void play() {
		if (step == 225) {
			MessageBox(NULL, "平局", "结果", MB_OK);
		}
		turn = step % 2;//通过步数确定谁落子（之前试过turn=!turn，但是会出现问题）
		if (computer) {
			turn = step % 2;
			if (XianShou) {//人先手，电脑白棋
				computercolor = whitec;
				if ((turn % 2 == 1)&&(!end)) {//轮到电脑下棋
					cpboard.cp();
					std::pair<int, int> aiPos = aiMove(cpboard, computercolor);
					placex[step + 1] = aiPos.first;
					placey[step + 1] = aiPos.second;
					turn = 1;
					update();
				}
				else {
					handleMouse();
				}
			}
			else {//人后手，电脑黑棋
				computercolor = blackc;
				if ((turn % 2 == 0)&&(!end)) {//轮到电脑下棋
					if (step == 0) {
						placex[step + 1] = 7;
						placey[step + 1] = 7;
						update();
					}
					else {
						cpboard.cp();
						std::pair<int, int> aiPos = aiMove(cpboard, computercolor);
						placex[step + 1] = aiPos.first;
						placey[step + 1] = aiPos.second;
						update();
					}
				}
				else {
					handleMouse();
				}
			}
		}
		else {
			handleMouse();
		}
	}

};
//飞天皮肤下棋暂停界面
class FeiTianStop {
private:
	IMAGE blur, lb;
	IMAGE r, rc, s, sc, e, ec, c, cc;
	Button retry, save, exit, costume;
public:
	FeiTianStop() {
		loadimage(&blur, "resources/blur.png");
		loadimage(&lb, "resources/little_background.png");
		loadimage(&r, "resources/retry.png");
		loadimage(&rc, "resources/retry_change.png");
		loadimage(&s, "resources/save.png");
		loadimage(&sc, "resources/save_change.png");
		loadimage(&e, "resources/exit.png");
		loadimage(&ec, "resources/exit_change.png");
		loadimage(&c, "resources/costume.png");
		loadimage(&cc, "resources/costume_change.png");
		inb(retry, 333, 239, 234, 98);
		inb(save, 333, 347, 234, 98);
		inb(exit, 333, 455, 234, 98);
		inb(costume, 333, 563, 234, 98);
		retry.image = r;
		retry.image_change = rc;
		save.image = s;
		save.image_change = sc;
		exit.image = e;
		exit.image_change = ec;
		costume.image = c;
		costume.image_change = cc;
	}
	~FeiTianStop() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		drawbutton(retry);
		drawbutton(save);
		drawbutton(exit);
		drawbutton(costume);
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		checkHover(&retry, msg.x, msg.y);
		checkHover(&save, msg.x, msg.y);
		checkHover(&exit, msg.x, msg.y);
		checkHover(&costume, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&retry, msg.x, msg.y);
			checkClick(&save, msg.x, msg.y);
			checkClick(&exit, msg.x, msg.y);
			checkClick(&costume, msg.x, msg.y);
			if (retry.isClicked) {
				std::cout << "retry clicked" << std::endl;
				step = 0;//重来
				PAGE = ftGame;
				retry.isHover = false;
			}
			else if (save.isClicked) {
				SAVE();//保存
				save.isHover = false;
				std::cout << "save clicked" << std::endl;
			}
			else if (exit.isClicked) {
				PAGE = ftStart;//返回
				step = 0;//重置（没点保存就退出，不负任何责任哈）
				exit.isHover = false;
				std::cout << "exit clicked" << std::endl;
			}
			else if (costume.isClicked) {
				std::cout << "costume clicked" << std::endl;
				PAGE = jxstop;//换皮肤
				costume.isHover = false;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return game" << std::endl;
				PAGE = ftGame;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}

	}
};

//第二个皮肤逻辑同上，不过多注释（因为几乎都是复制粘贴0
class JiXieStartPage{
private:
	IMAGE t1, t2, t3, t4, t5;
	int title = 2;
	IMAGE spb;
	Button start, setting, rule;
	IMAGE st, stc, se, sec, ru, ruc;
	int time = 0, add = 0;
public:
	JiXieStartPage()
	{
		loadimage(&spb, "resources/JX_start_page_background.png");
		loadimage(&t1, "resources/JX_title1.png");
		loadimage(&t2, "resources/JX_title2.png");
		loadimage(&t3, "resources/JX_title3.png");
		loadimage(&t4, "resources/JX_title4.png");
		loadimage(&t5, "resources/JX_title5.png");
		loadimage(&st, "resources/JX_start.png");
		loadimage(&stc, "resources/JX_start_change.png");
		loadimage(&se, "resources/JX_setting.png");
		loadimage(&sec, "resources/JX_setting_change.png");
		loadimage(&ru, "resources/JX_rule.png");
		loadimage(&ruc, "resources/JX_rule_change.png");
		inb(rule, 40, 540, 270, 167);
		inb(start, 315, 540, 270, 167);
		inb(setting, 590, 540, 270, 167);
		rule.image = ru;
		rule.image_change = ruc;
		start.image = st;
		start.image_change = stc;
		setting.image = se;
		setting.image_change = sec;
	};
	~JiXieStartPage() {};
	void draw() {
		putimage(0, 0, &spb);
		switch (title) {
		case 1:transparentimage(NULL, 210, 105, &t1); break;
		case 2:transparentimage(NULL, 210, 105, &t2); break;
		case 3:transparentimage(NULL, 210, 105, &t3); break;
		case 4:transparentimage(NULL, 210, 105, &t4); break;
		case 5:transparentimage(NULL, 210, 105, &t5); break;
		default:break;
		}
		transdrawbutton(rule);
		transdrawbutton(start);
		transdrawbutton(setting);
	}
	void update(){
		time++;
		time %= 20;
		//使标题有故障效果
		if (time == 0) {
			add ++;
			add %= 35;
			if (add < 4) title = 1;
			else if (4<=add&&add < 6) title = 2;
			else if (6<=add&&add < 8) title = 5;
			else if (8<=add&&add < 10) title = 1;
			else if (10<=add&&add < 13) title = 3;
			else if (13<=add&&add < 15) title = 1;
			else if (15<=add&&add < 16) title = 4;
			else if (16<=add&&add < 19) title = 1;
			else if (19<=add&&add < 21) title = 3;
			else if (21<=add&&add < 25) title = 2;
			else if (25<=add&&add < 28) title = 4;
			else if (28<=add&&add < 30) title = 5;
			else if (30<=add&&add < 33) title = 1;
			else if (33<=add&&add < 35) title = 4;
		}
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		checkHover(&start, msg.x, msg.y);
		checkHover(&setting, msg.x, msg.y);
		checkHover(&rule, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&start, msg.x, msg.y);
			checkClick(&setting, msg.x, msg.y);
			checkClick(&rule, msg.x, msg.y);
			if (start.isClicked) {
				std::cout << "start clicked" << std::endl;
				PAGE = jxready;
				start.isHover = false;
			}
			else if (setting.isClicked) {
				std::cout << "setting clicked" << std::endl;
				PAGE = jxSetting;
				setting.isHover = false;
			}
			else if (rule.isClicked) {
				std::cout << "rule clicked" << std::endl;
				PAGE = jxRule;
				rule.isHover = false;
			}
			else std::cout << "nothing happend" << std::endl;
			break;
		}
	}
};

class JiXieReady {
private:
	IMAGE blur, lb;
	IMAGE n, nc, s, sc;
	Button newstart, save;
	int pagestep = 1;
	IMAGE vp, vpc, vc, vcc;
	Button pvp, pvc;
	IMAGE f, fc, l, lc;
	Button first, last;
	
public:
	JiXieReady() {
		loadimage(&blur, "resources/JX_blur.png");
		loadimage(&lb, "resources/JX_little_background.png");
		loadimage(&n, "resources/JX_new.png");
		loadimage(&nc, "resources/JX_new_change.png");
		loadimage(&s, "resources/JX_read.png");
		loadimage(&sc, "resources/JX_read_change.png");
		loadimage(&vp, "resources/JX_pvp.png");
		loadimage(&vpc, "resources/JX_pvp_change.png");
		loadimage(&vc, "resources/JX_pvc.png");
		loadimage(&vcc, "resources/JX_pvc_change.png");
		loadimage(&f, "resources/JX_xs.png");
		loadimage(&fc, "resources/JX_xs_change.png");
		loadimage(&l, "resources/JX_hs.png");
		loadimage(&lc, "resources/JX_hs_change.png");
		inb(newstart, 326, 315, 214, 105);
		inb(save, 326, 452, 214, 105);
		inb(pvp, 180, 185, 214, 105);
		inb(pvc, 472, 185, 214, 105);
		inb(first, 180, 587, 214, 105);
		inb(last, 472, 587, 214, 105);
		newstart.image = n;
		newstart.image_change = nc;
		save.image = s;
		save.image_change = sc;
		pvp.image = vp;
		pvp.image_change = vpc;
		pvc.image = vc;
		pvc.image_change = vcc;
		first.image = f;
		first.image_change = fc;
		last.image = l;
		last.image_change = lc;
	}
	~JiXieReady() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		transdrawbutton(newstart);
		transdrawbutton(save);
		if (pagestep == 2) {
			transparentimage(NULL, 0, 0, &blur);
			transdrawbutton(pvp);
			transdrawbutton(pvc);
		}
		if (pagestep == 3) {
			transparentimage(NULL, 0, 0, &blur);
			transdrawbutton(pvp);
			transdrawbutton(pvc);
			transparentimage(NULL, 0, 0, &blur);
			transdrawbutton(first);
			transdrawbutton(last);
		}
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		if (pagestep == 1) {
			checkHover(&newstart, msg.x, msg.y);
			checkHover(&save, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&newstart, msg.x, msg.y);
				checkClick(&save, msg.x, msg.y);
				if (newstart.isClicked) {
					step = 0;
					pagestep = 2;
					newstart.isHover = false;
					std::cout << "newstart clicked" << std::endl;
				}
				else if (save.isClicked) {
					READ();
					pagestep = 2;
					save.isHover = false;
					std::cout << "read clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					std::cout << "return" << std::endl;
					PAGE = jxStart;
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}
		else if (pagestep == 2) {
			checkHover(&pvp, msg.x, msg.y);
			checkHover(&pvc, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&pvp, msg.x, msg.y);
				checkClick(&pvc, msg.x, msg.y);
				if (pvp.isClicked) {
					computer = false;//不运行人工智能
					PAGE = jxGame;
					pagestep = 1;
					pvp.isHover = false;
					std::cout << "pvp clicked" << std::endl;
				}
				else if (pvc.isClicked) {
					computer = true;//运行人工智能
					pagestep = 3;
					pvc.isHover = false;
					std::cout << "pvc clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep=1;
					std::cout << "return pagestep2" << std::endl;
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}
		else if (pagestep == 3) {
			checkHover(&first, msg.x, msg.y);
			checkHover(&last, msg.x, msg.y);
			switch (msg.uMsg) {
			case WM_MOUSEMOVE:break;
			case WM_LBUTTONDOWN:
				checkClick(&first, msg.x, msg.y);
				checkClick(&last, msg.x, msg.y);
				if (first.isClicked) {
					XianShou = true;//先手
					PAGE = jxGame;
					pagestep = 1;
					first.isHover = false;
					std::cout << "first clicked" << std::endl;
				}
				else if (last.isClicked) {
					XianShou = false;//后手
					PAGE = jxGame;
					pagestep = 1;
					last.isHover = false;
					std::cout << "last clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep = 2;
					std::cout << "return pagestep1" << std::endl;
				}
				else {
					std::cout << "nothing happend" << std::endl;
				}
				break;
			}
		}

	}
};

class JiXieRule{
private:
	IMAGE blur;
	IMAGE r1, r2, r3, r4;
	IMAGE n, nc, p, pc;
	Button next, previous;
	int page = 1;
public:
	JiXieRule() {
		loadimage(&blur, "resources/JX_blur.png");
		loadimage(&r1, "resources/JX_rule1.png");
		loadimage(&r2, "resources/JX_rule2.png");
		loadimage(&r3, "resources/JX_rule3.png");
		loadimage(&r4, "resources/JX_rule4.png");
		loadimage(&n, "resources/JX_next.png");
		loadimage(&nc, "resources/JX_next_change.png");
		loadimage(&p, "resources/JX_previous.png");
		loadimage(&pc, "resources/JX_previous_change.png");
		inb(previous, 158, 632, 230, 111);
		inb(next, 528, 632, 230, 111);
		next.image = n;
		next.image_change = nc;
		previous.image = p;
		previous.image_change = pc;
	}
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		if (page == 1) {
			putimage(150, 150, &r1);
			transdrawbutton(next);
		}
		else if (page == 2) {
			putimage(150, 150, &r2);
			transdrawbutton(next);
			transdrawbutton(previous);
		}
		else if (page == 3) {
			putimage(150, 150, &r3);
			transdrawbutton(next);
			transdrawbutton(previous);
		}
		else if (page == 4) {
			putimage(150, 150, &r4);
			transdrawbutton(previous);
		}
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		if (page == 1) {
			checkHover(&next, msg.x, msg.y);
		}
		else if ((page == 2) || (page == 3)) {
			checkHover(&next, msg.x, msg.y);
			checkHover(&previous, msg.x, msg.y);
		}
		else if (page == 4) {
			checkHover(&previous, msg.x, msg.y);
		}
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&next, msg.x, msg.y);
			checkClick(&previous, msg.x, msg.y);
			if (next.isClicked) {
				std::cout << "next clicked" << std::endl;
				page++;
				previous.isHover = false;
				next.isHover = false;
			}
			else if (previous.isClicked) {
				std::cout << "previous clicked" << std::endl;
				page--;
				previous.isHover = false;
				next.isHover = false;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return" << std::endl;
				PAGE = jxStart;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}
		if (page < 1) {
			page = 1;
		}
		if (page > 4) {
			page = 4;
		}
	}
};

class JiXieSetting {
private:
	IMAGE blur, lb;
	//IMAGE c, cc;
	//Button costume;
public:
	JiXieSetting() {
		loadimage(&blur, "resources/JX_blur.png");
		loadimage(&lb, "resources/hybriground.png");
		//loadimage(&c, "resources/costume.png");
		//loadimage(&cc, "resources/costume_change.png");
		//inb(costume, 307, 282, 286, 120);
		//costume.image = c;
		//costume.image_change = cc;
	}
	~JiXieSetting() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		//drawbutton(costume);
	}
	void handleMouse() {
		draw();
		MOUSEMSG msg = GetMouseMsg();
		//checkHover(&costume, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			/*checkClick(&costume, msg.x, msg.y);
			if (costume.isClicked) {
				std::cout << "costume clicked" << std::endl;
				PAGE = ftSetting;
			}*/
			if (msg.x >= 150 && msg.x <= 750 && msg.y >= 150 && msg.y <= 750) {
				std::cout << "costume clicked" << std::endl;
				PAGE = ftSetting;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return game" << std::endl;
				PAGE = jxStart;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}
	}
};

class JiXieBoard{
private:
	IMAGE board, black, white;
	bool turn = false;//0 blackc,1 whitec
	bool shouldbreak = false;
	IMAGE r, rc, s, sc;
	Button regret, stop;
	condition con = notForbidden;
	state computercolor = blackc;
	Board cpboard;
public:
	JiXieBoard() {
		loadimage(&board, "resources/JX_chessboard.png");
		loadimage(&black, "resources/JX_black_chess.png");
		loadimage(&white, "resources/JX_white_chess.png");
		loadimage(&r, "resources/JX_regret.png");
		loadimage(&rc, "resources/JX_regret_change.png");
		loadimage(&s, "resources/JX_stop.png");
		loadimage(&sc, "resources/JX_stop_change.png");
		inb(regret, 735, 385, 130, 130);
		inb(stop, 35, 385, 130, 130);
		regret.image = r;
		regret.image_change = rc;
		stop.image = s;
		stop.image_change = sc;
	}
	~JiXieBoard() {};
	void draw() {
		putimage(0, 0, &board);
		if (step > 0) {
			int x = 0;
			int y = 0;
			if (end) {
				setfillcolor(RGB(139, 0, 18));
				setlinecolor(RGB(139, 0, 18));
				for (int i = 0; i < 5; ++i) {
					x = 175 + winx[i] * 550 / 14;
					y = 175 + winy[i] * 550 / 14;
					fillcircle(x, y, R + 2);
				}
			}
			x = 175 + placex[step] * 550 / 14;
			y = 175 + placey[step] * 550 / 14;
			setfillcolor(RGB(224, 203, 109));
			setlinecolor(RGB(224, 203, 109));
			fillcircle(x, y, R + 1);
		}
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				if (BOARD[step][i][j] == blackc) {//blackc 1
					int x = 175 + i * 550 / 14 - R;
					int y = 175 + j * 550 / 14 - R;
					transparentimage(NULL, x, y, &black);
				}
				else if (BOARD[step][i][j] == whitec) {//whitec 2
					int x = 175 + i * 550 / 14 - R;
					int y = 175 + j * 550 / 14 - R;
					transparentimage(NULL, x, y, &white);
				}
			}
		}
		transdrawbutton(regret);
		transdrawbutton(stop);
	}
	void update() {
		step++;
		for (int i = 0; i < BOARD_SIZE; ++i) {
			for (int j = 0; j < BOARD_SIZE; ++j) {
				BOARD[step][i][j] = BOARD[step - 1][i][j];
			}
		}
		if (turn == 0) {
			BOARD[step][placex[step]][placey[step]] = blackc;
		}
		else {
			BOARD[step][placex[step]][placey[step]] = whitec;
		}
		//下完棋后判断是否胜利
		if (con == blackwin) {
			end = true;
		}
		else if (Win(turn)) {
			end = true;
		}
		else {
			turn = step % 2;
		}
	}
	void handleMouse() {
		turn = step % 2;
		MOUSEMSG msg = GetMouseMsg();
		//光标挪动到可下子位置会显示轮廓
		if (!end) {
			for (int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					if (BOARD[step][i][j] == empty) {//empty 0
						int x = 175 + i * 550 / 14;
						int y = 175 + j * 550 / 14;
						if ((msg.x >= x - R) && (msg.x <= x + R) && (msg.y >= y - R) && (msg.y <= y + R)) {
							setfillcolor(WHITE);
							setlinecolor(WHITE);
							circle(x, y, R);
						}
					}
				}
			}
		}
		checkHover(&regret, msg.x, msg.y);
		checkHover(&stop, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&regret, msg.x, msg.y);
			checkClick(&stop, msg.x, msg.y);
			//判断是否点击悔棋
			if (regret.isClicked) {
				std::cout << "regret clicked" << std::endl;
				if (step > 0) {
					if (computer) {
						if (((XianShou) && (computercolor == whitec)) || ((!XianShou) && (computercolor = blackc))) {
							initialize_k(step);
							step--;
							if (step < 0) {
								step = 0;
							}
							initialize_k(step);
							step--;
						}

					}
					else {
						initialize_k(step);
						step--;
					}
					if (step < 0) {
						step = 0;
					}
					if (con = blackwin) {
						con = notForbidden;
					}
					end = false;
				}
				regret.isHover = false;
			}
			//判断时候点击暂停
			else if (stop.isClicked) {
				std::cout << "stop clicked" << std::endl;
				PAGE = jxstop;
				stop.isHover = false;
			}
			//非分出胜负时，检索下棋位置
			else if (!end) {
				for (int i = 0; i < BOARD_SIZE; ++i) {
					for (int j = 0; j < BOARD_SIZE; ++j) {
						if (BOARD[step][i][j] == empty) {//empty 0
							int x = 175 + i * 550 / 14;
							int y = 175 + j * 550 / 14;
							if ((msg.x >= x - R) && (msg.x <= x + R) && (msg.y >= y - R) && (msg.y <= y + R)) {
								if (!turn) {
									con = ForbiddenCheck(i, j);
									if (con == doubleFour) {
										std::cout << "四四禁手" << std::endl;
										MessageBox(NULL, "四四禁手", "禁手提醒", MB_OK);
										break;
									}
									else if (con == doubleThree) {
										std::cout << "三三禁手" << std::endl;
										MessageBox(NULL, "三三禁手", "禁手提醒", MB_OK);
										break;
									}
									else if (con == longConnection) {
										std::cout << "长连禁手" << std::endl;
										MessageBox(NULL, "长连禁手", "禁手提醒", MB_OK);
										break;
									}

								}
								placex[step + 1] = i;
								placey[step + 1] = j;
								shouldbreak = true;
								break;
							}
						}
					}
					if (shouldbreak)break;
				}
				if (shouldbreak) {
					update();
					shouldbreak = false;
				}
			}
			break;
		}
	}
	void play() {
		if (step == 225) {
			MessageBox(NULL, "平局", "结果", MB_OK);
		}
		turn = step % 2;//通过步数确定谁落子（之前试过turn=!turn，但是会出现问题）
		if (computer) {
			turn = step % 2;
			if (XianShou) {//人先手，电脑白棋
				computercolor = whitec;
				if ((turn % 2 == 1) && (!end)) {//轮到电脑下棋
					cpboard.cp();
					std::pair<int, int> aiPos = aiMove(cpboard, computercolor);
					placex[step + 1] = aiPos.first;
					placey[step + 1] = aiPos.second;
					turn = 1;
					update();
				}
				else {
					handleMouse();
				}
			}
			else {//人后手，电脑黑棋
				computercolor = blackc;
				if ((turn % 2 == 0) && (!end)) {//轮到电脑下棋
					if (step == 0) {
						placex[step + 1] = 7;
						placey[step + 1] = 7;
						update();
					}
					else {
						cpboard.cp();
						std::pair<int, int> aiPos = aiMove(cpboard, computercolor);
						placex[step + 1] = aiPos.first;
						placey[step + 1] = aiPos.second;
						update();
					}
				}
				else {
					handleMouse();
				}
			}
		}
		else {
			handleMouse();
		}
	}

};

class JiXieStop {
private:
	IMAGE blur, lb;
	IMAGE r, rc, s, sc, e, ec, c, cc;
	Button retry, save, exit, costume;
public:
	JiXieStop() {
		loadimage(&blur, "resources/JX_blur.png");
		loadimage(&lb, "resources/JX_little_background.png");
		loadimage(&r, "resources/JX_retry.png");
		loadimage(&rc, "resources/JX_retry_change.png");
		loadimage(&s, "resources/JX_save.png");
		loadimage(&sc, "resources/JX_save_change.png");
		loadimage(&e, "resources/JX_exit.png");
		loadimage(&ec, "resources/JX_exit_change.png");
		loadimage(&c, "resources/costume.png");
		loadimage(&cc, "resources/costume_change.png");
		inb(retry, 333, 200, 256, 125);
		inb(save, 333, 315, 256, 125);
		inb(exit, 333, 425, 256, 125);
		inb(costume, 333, 563, 234, 98);
		retry.image = r;
		retry.image_change = rc;
		save.image = s;
		save.image_change = sc;
		exit.image = e;
		exit.image_change = ec;
		costume.image = c;
		costume.image_change = cc;
	}
	~JiXieStop() {};
	void draw() {
		transparentimage(NULL, 0, 0, &blur);
		transparentimage(NULL, 0, 0, &blur);
		putimage(150, 150, &lb);
		transdrawbutton(retry);
		transdrawbutton(save);
		transdrawbutton(exit);
		drawbutton(costume);
	}
	void handleMouse() {
		MOUSEMSG msg = GetMouseMsg();
		JX_checkHover(&retry, msg.x, msg.y, 33);
		JX_checkHover(&save, msg.x, msg.y, 33);
		JX_checkHover(&exit, msg.x, msg.y, 33);
		checkHover(&costume, msg.x, msg.y);
		switch (msg.uMsg) {
		case WM_MOUSEMOVE:break;
		case WM_LBUTTONDOWN:
			checkClick(&retry, msg.x, msg.y);
			checkClick(&save, msg.x, msg.y);
			checkClick(&exit, msg.x, msg.y);
			checkClick(&costume, msg.x, msg.y);
			if (retry.isClicked) {
				std::cout << "retry clicked" << std::endl;
				step = 0;
				PAGE = jxGame;
				retry.isHover = false;
			}
			else if (save.isClicked) {
				SAVE();
				save.isHover = false;
				std::cout << "save clicked" << std::endl;
			}
			else if (exit.isClicked) {
				PAGE = jxStart;
				step = 0;
				exit.isHover = false;
				std::cout << "exit clicked" << std::endl;
			}
			else if (costume.isClicked) {
				std::cout << "costume clicked" << std::endl;
				costume.isHover = false;
				PAGE = ftstop;
			}
			else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
				std::cout << "return game" << std::endl;
				PAGE = jxGame;
			}
			else {
				std::cout << "nothing happend" << std::endl;
			}
			break;
		}

	}
};

//运行
void run() {
	FeiTianStartPage ftsp;
	FeiTianReady ftrd;
	FeiTianRule ftr;
	FeiTianSetting ftst;
	FeiTianBoard ftb;
	FeiTianStop ftsto;

	JiXieStartPage jxsp;
	JiXieReady jxrd;
	JiXieRule jxr;
	JiXieSetting jxst;
	JiXieBoard jxb;
	JiXieStop jxsto;
	while (true) {
		//双缓存绘图，需要放在绘图代码之前和之后
		BeginBatchDraw();//开始批量绘图

		if (PAGE == ftStart || PAGE == ftready || PAGE == ftRule || PAGE == ftSetting) {
			ftsp.draw();
			if (ftsp.shouldGo()) {
				if (PAGE == ftRule) {
					ftr.draw();
					ftr.handleMouse();
				}

				else if (PAGE == ftready) {
					ftrd.draw();
					ftrd.handleMouse();
				}

				else if (PAGE == ftSetting) {
					ftst.handleMouse();
				}

				else {
					ftsp.update();
					ftsp.handleMouse();
				}
			}
			else {
				ftsp.update();
			}
		}
		else if ((PAGE == ftGame) || (PAGE == ftstop)) {
			ftb.draw();
			if (PAGE == ftstop) {
				ftsto.draw();
				ftsto.handleMouse();
			}
			else {
				ftb.play();
			}
		}

		else if (PAGE == jxStart || PAGE == jxready || PAGE == jxRule || PAGE == jxSetting) {
			jxsp.draw();
			if (PAGE == jxRule) {
				jxr.draw();
				jxr.handleMouse();
			}

			else if (PAGE == jxready) {
				jxrd.draw();
				jxrd.handleMouse();
			}

			else if (PAGE == jxSetting) {
				jxst.handleMouse();
			}

			else {
				jxsp.update();
				jxsp.handleMouse();
			}
		}
		else if ((PAGE == jxGame) || (PAGE == jxstop)) {
			jxb.draw();
			if (PAGE == jxstop) {
				jxsto.draw();
				jxsto.handleMouse();
			}
			else {
				jxb.play();
			}
		}

		FlushBatchDraw();//清空批量绘图

	}

	closegraph();
	PostQuitMessage(0);
}

int main() {

	initgraph(900, 900, EW_SHOWCONSOLE);

	HWND hnd = GetHWnd();
	SetWindowText(hnd, "五子棋");

	run();
	
	return 0;
}
