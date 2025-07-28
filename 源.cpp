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

#define BOARD_SIZE 15//���̳ߴ�
const int R = 19;//���Ӱ뾶

//��������ҳ��
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
}BOARD[226][BOARD_SIZE][BOARD_SIZE];//���

//��¼����
int placex[226] = { 0 };
int placey[226] = { 0 };

//��¼Ӯ��
int winx[5] = { 0 };
int winy[5] = { 0 };

//��ʼ������
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

int step = 0;//��Ĳ���
bool end = false;//�Ƿ����

//�ж�ʤ��
bool Win(bool turn) {
	//0 blackc,1 whitec
	state color;
	if (turn) color = whitec;
	else color = blackc;
	//����
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
	//����
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
	//��������
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
	//��������
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
//�������

//condition KeyPointForbiddenCheck(int x, int y, int adjsame, int direction);

condition ForbiddenCheck(int x,int y) {
	if (step % 2 == 1) return notBlack;//���岻����

	//�����±��ʾ����
	/*
	����7 ����0 ����1
	����6       ����2
	����5 ����4 ����3
	*/
	int adjsame[8] = { 0 };//��(x,y)�������������� adjacent
	int adjempty[8] = { 0 };//adjsame������������λ��
	int jumpsame[8] = { 0 };//adjempty������������
	int jumpempty[8] = { 0 };//jumpsame��������λ��
	int jumpjumpsame[8] = { 0 };//jumpempty������������
	
	int _x = x, _y = y;

	//����
	for (_y = y - 1; (_y >= 0) && (BOARD[step][x][_y] == blackc); _y--, adjsame[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == empty; _y--, adjempty[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == blackc; _y--, jumpsame[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == empty; _y--, jumpempty[0]++);
	for (; (_y >= 0) && BOARD[step][x][_y] == blackc; _y--, jumpjumpsame[0]++);

	//����
	for (_x = x + 1,_y = y - 1; (_x < 15)&&(_y >= 0) && (BOARD[step][_x][_y] == blackc);_x++, _y--, adjsame[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x++, _y--, adjempty[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x++, _y--, jumpsame[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x++, _y--, jumpempty[1]++);
	for (; (_x < 15) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x++, _y--, jumpjumpsame[1]++);

	//����
	for (_x = x + 1; (_x < 15) && (BOARD[step][_x][y] == blackc); _x++, adjsame[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == empty; _x++, adjempty[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == blackc; _x++, jumpsame[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == empty; _x++, jumpempty[2]++);
	for (; (_x < 15) && BOARD[step][_x][y] == blackc; _x++, jumpjumpsame[2]++);

	//����
	for (_x = x + 1, _y = y + 1; (_x < 15) && (_y < 15) && (BOARD[step][_x][_y] == blackc); _x++, _y++, adjsame[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == empty; _x++, _y++, adjempty[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x++, _y++, jumpsame[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == empty; _x++, _y++, jumpempty[3]++);
	for (; (_x < 15) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x++, _y++, jumpjumpsame[3]++);

	//����
	for (_y = y + 1; (_y < 15) && (BOARD[step][x][_y] == blackc); _y++, adjsame[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == empty; _y++, adjempty[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == blackc; _y++, jumpsame[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == empty; _y++, jumpempty[4]++);
	for (; (_y < 15) && BOARD[step][x][_y] == blackc; _y++, jumpjumpsame[4]++);

	//����
	for (_x = x - 1, _y = y + 1; (_x >= 0) && (_y < 15) && (BOARD[step][_x][_y] == blackc); _x--, _y++, adjsame[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == empty; _x--, _y++, adjempty[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x--, _y++, jumpsame[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == empty; _x--, _y++, jumpempty[5]++);
	for (; (_x >= 0) && (_y < 15) && BOARD[step][_x][_y] == blackc; _x--, _y++, jumpjumpsame[5]++);

	//����
	for (_x = x - 1; (_x >= 0) && (BOARD[step][_x][y] == blackc); _x--, adjsame[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == empty; _x--, adjempty[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == blackc; _x--, jumpsame[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == empty; _x--, jumpempty[6]++);
	for (; (_x >= 0) && BOARD[step][_x][y] == blackc; _x--, jumpjumpsame[6]++);

	//����
	for (_x = x - 1, _y = y - 1; (_x >= 0) && (_y >= 0) && (BOARD[step][_x][_y] == blackc); _x--, _y--, adjsame[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x--, _y--, adjempty[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x--, _y--, jumpsame[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == empty; _x--, _y--, jumpempty[7]++);
	for (; (_x >= 0) && (_y >= 0) && BOARD[step][_x][_y] == blackc; _x--, _y--, jumpjumpsame[7]++);

	//�ȼ���Ƿ�����壬�������壬�����ڽ��֣������ʤ
	/*
	�˸�����
	����7 ����0 ����1
	����6       ����2
	����5 ����4 ����3

	������
	0   ��ֱ   ����
	1 �������� �L�J
	2   ˮƽ   ����
	3 �������� �I�K
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

	//�������������жϡ�������
	int three = 0, four = 0;//������������+������


	for (int i = 0; i < 4; ++i) {
		//��������
		if (adjsame[i] + adjsame[i + 4] >= 5) return longConnection;//������������

		//����������?�����?
		// ?ָ�������������
		else if (adjsame[i] + adjsame[i + 4] == 3) {

			//���ġ������ж�
			bool isFour = false;

			//?�ա����?
			if (adjempty[i] > 0) { 
				//ͨ���ݹ��жϹؼ����Ƿ����
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				isFour = true;
			}

			//?������?
			if (adjempty[i + 4] > 0) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				isFour = true;
			}
			if (isFour) four++;
		}

		//����������?����?
		else if (adjsame[i] + adjsame[i + 4] == 2) {

			//���ġ������ж�
			
			//?��ա���?
			if (adjempty[i] == 1 && jumpsame[i] == 1) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}
			//?����ա�?
			if (adjempty[i+4] == 1 && jumpsame[i+4] == 1) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//�����ж�
			bool isThree = false;

			//?�տա���գ�
			if (((adjempty[i] > 2) || ((adjempty[i] == 2) && (jumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				isThree = true;
			}

			//?�ա���տգ�
			if (((adjempty[i + 4] > 2) || ((adjempty[i + 4] == 2) && (jumpsame[i] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				isThree = true;
			}

			if(isThree) three++;
		}

		//�������� ?���?
		else if (adjsame[i] + adjsame[i + 4] == 1) {

			//���ġ������ж�

			//?���ա��?
			if (adjempty[i] == 1 && jumpsame[i] == 2) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}
			if (adjempty[i+4] == 1 && jumpsame[i+4] == 2) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//�����ж�

			//?�ա�ա���?
			if ((adjempty[i] == 1) && (adjsame[i+4] == 1) && ((jumpempty[i+4] > 1) || ((jumpempty[i+4] == 1) && (jumpjumpsame[i+4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				three++;
			}
			if ((adjempty[i+4] == 1) && (adjsame[i] == 1) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				three++;
			}
		}

		//һ�� ?��?
		else if (adjsame[i] + adjsame[i + 4] == 0) {

			//���ġ������ж�

			//?����ա�
			if (adjempty[i] == 1 && jumpsame[i] == 3) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				four++;
			}

			//?��ա���?
			if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 3) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
				four++;
			}

			//�����ж�

			//?�ա��ա�գ�
			if ((adjempty[i] == 1) && (jumpsame[i] == 2) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
				three++;
			}

			//?�ա�ա���?
			if ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 2) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i + 4] == 0)))) {
				//if (KeyPointForbiddenCheck(x, y, adjsame[i + 4], i + 4) == notForbidden) 
				three++;
			}
		}
	}
	//�������������жϽ�����������

	if (four > 1) return doubleFour;
	if (three > 1)return doubleThree;
	return notForbidden;
}

/*condition KeyPointForbiddenCheck(int x, int y, int adjsame, int direction) {
	int i = 0, j = 0;
	adjsame++;
	if (direction >= 4)adjsame = -adjsame;

	//����ؼ�������
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

	//�������з�������
	BOARD[step][x][y] = blackc;
	BOARD[step][x][y] = blackc;

	//���ؼ���
	condition result = ForbiddenCheck(i, j);

	//��ԭ
	BOARD[step][x][y] = empty;
	BOARD[step][x][y] = empty;

	return result;
}*/

bool computer = false;//�Ƿ��˻���ս
bool XianShou = true;//�Ƿ�����

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
	//��ֱ�Ӹ���֮ǰ��
	condition ForbiddenCheck(int x, int y) {
		if (step % 2 == 1) return notBlack;//���岻����

		//�����±��ʾ����
		/*
		����7 ����0 ����1
		����6       ����2
		����5 ����4 ����3
		*/
		int adjsame[8] = { 0 };//��(x,y)�������������� adjacent
		int adjempty[8] = { 0 };//adjsame������������λ��
		int jumpsame[8] = { 0 };//adjempty������������
		int jumpempty[8] = { 0 };//jumpsame��������λ��
		int jumpjumpsame[8] = { 0 };//jumpempty������������

		int _x = x, _y = y;

		//����
		for (_y = y - 1; (_y >= 0) && (board[x][_y] == blackc); _y--, adjsame[0]++);
		for (; (_y >= 0) && board[x][_y] == empty; _y--, adjempty[0]++);
		for (; (_y >= 0) && board[x][_y] == blackc; _y--, jumpsame[0]++);
		for (; (_y >= 0) && board[x][_y] == empty; _y--, jumpempty[0]++);
		for (; (_y >= 0) && board[x][_y] == blackc; _y--, jumpjumpsame[0]++);

		//����
		for (_x = x + 1, _y = y - 1; (_x < 15) && (_y >= 0) && (board[_x][_y] == blackc); _x++, _y--, adjsame[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == empty; _x++, _y--, adjempty[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == blackc; _x++, _y--, jumpsame[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == empty; _x++, _y--, jumpempty[1]++);
		for (; (_x < 15) && (_y >= 0) && board[_x][_y] == blackc; _x++, _y--, jumpjumpsame[1]++);

		//����
		for (_x = x + 1; (_x < 15) && (board[_x][y] == blackc); _x++, adjsame[2]++);
		for (; (_x < 15) && board[_x][y] == empty; _x++, adjempty[2]++);
		for (; (_x < 15) && board[_x][y] == blackc; _x++, jumpsame[2]++);
		for (; (_x < 15) && board[_x][y] == empty; _x++, jumpempty[2]++);
		for (; (_x < 15) && board[_x][y] == blackc; _x++, jumpjumpsame[2]++);

		//����
		for (_x = x + 1, _y = y + 1; (_x < 15) && (_y < 15) && (board[_x][_y] == blackc); _x++, _y++, adjsame[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == empty; _x++, _y++, adjempty[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == blackc; _x++, _y++, jumpsame[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == empty; _x++, _y++, jumpempty[3]++);
		for (; (_x < 15) && (_y < 15) && board[_x][_y] == blackc; _x++, _y++, jumpjumpsame[3]++);

		//����
		for (_y = y + 1; (_y < 15) && (board[x][_y] == blackc); _y++, adjsame[4]++);
		for (; (_y < 15) && board[x][_y] == empty; _y++, adjempty[4]++);
		for (; (_y < 15) && board[x][_y] == blackc; _y++, jumpsame[4]++);
		for (; (_y < 15) && board[x][_y] == empty; _y++, jumpempty[4]++);
		for (; (_y < 15) && board[x][_y] == blackc; _y++, jumpjumpsame[4]++);

		//����
		for (_x = x - 1, _y = y + 1; (_x >= 0) && (_y < 15) && (board[_x][_y] == blackc); _x--, _y++, adjsame[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == empty; _x--, _y++, adjempty[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == blackc; _x--, _y++, jumpsame[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == empty; _x--, _y++, jumpempty[5]++);
		for (; (_x >= 0) && (_y < 15) && board[_x][_y] == blackc; _x--, _y++, jumpjumpsame[5]++);

		//����
		for (_x = x - 1; (_x >= 0) && (board[_x][y] == blackc); _x--, adjsame[6]++);
		for (; (_x >= 0) && board[_x][y] == empty; _x--, adjempty[6]++);
		for (; (_x >= 0) && board[_x][y] == blackc; _x--, jumpsame[6]++);
		for (; (_x >= 0) && board[_x][y] == empty; _x--, jumpempty[6]++);
		for (; (_x >= 0) && board[_x][y] == blackc; _x--, jumpjumpsame[6]++);

		//����
		for (_x = x - 1, _y = y - 1; (_x >= 0) && (_y >= 0) && (board[_x][_y] == blackc); _x--, _y--, adjsame[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == empty; _x--, _y--, adjempty[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == blackc; _x--, _y--, jumpsame[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == empty; _x--, _y--, jumpempty[7]++);
		for (; (_x >= 0) && (_y >= 0) && board[_x][_y] == blackc; _x--, _y--, jumpjumpsame[7]++);

		//�ȼ���Ƿ�����壬�������壬�����ڽ��֣������ʤ
		/*
		�˸�����
		����7 ����0 ����1
		����6       ����2
		����5 ����4 ����3

		������
		0   ��ֱ   ����
		1 �������� �L�J
		2   ˮƽ   ����
		3 �������� �I�K
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

		//�������������жϡ�������
		int three = 0, four = 0;//������������+������


		for (int i = 0; i < 4; ++i) {
			//��������
			if (adjsame[i] + adjsame[i + 4] >= 5) return longConnection;//������������

			//����������?�����?
			// ?ָ�������������
			else if (adjsame[i] + adjsame[i + 4] == 3) {

				//���ġ������ж�
				bool isFour = false;

				//?�ա����?
				if (adjempty[i] > 0) {
					//ͨ���ݹ��жϹؼ����Ƿ����
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					isFour = true;
				}

				//?������?
				if (adjempty[i + 4] > 0) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					isFour = true;
				}
				if (isFour) four++;
			}

			//����������?����?
			else if (adjsame[i] + adjsame[i + 4] == 2) {

				//���ġ������ж�

				//?��ա���?
				if (adjempty[i] == 1 && jumpsame[i] == 1) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}
				//?����ա�?
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 1) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//�����ж�
				bool isThree = false;

				//?�տա���գ�
				if (((adjempty[i] > 2) || ((adjempty[i] == 2) && (jumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					isThree = true;
				}

				//?�ա���տգ�
				if (((adjempty[i + 4] > 2) || ((adjempty[i + 4] == 2) && (jumpsame[i] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					isThree = true;
				}

				if (isThree) three++;
			}

			//�������� ?���?
			else if (adjsame[i] + adjsame[i + 4] == 1) {

				//���ġ������ж�

				//?���ա��?
				if (adjempty[i] == 1 && jumpsame[i] == 2) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 2) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//�����ж�

				//?�ա�ա���?
				if ((adjempty[i] == 1) && (adjsame[i + 4] == 1) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					three++;
				}
				if ((adjempty[i + 4] == 1) && (adjsame[i] == 1) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					three++;
				}
			}

			//һ�� ?��?
			else if (adjsame[i] + adjsame[i + 4] == 0) {

				//���ġ������ж�

				//?����ա�
				if (adjempty[i] == 1 && jumpsame[i] == 3) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					four++;
				}

				//?��ա���?
				if (adjempty[i + 4] == 1 && jumpsame[i + 4] == 3) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i+4], i+4) == notForbidden) 
					four++;
				}

				//�����ж�

				//?�ա��ա�գ�
				if ((adjempty[i] == 1) && (jumpsame[i] == 2) && ((jumpempty[i] > 1) || ((jumpempty[i] == 1) && (jumpjumpsame[i] == 0))) && ((adjempty[i + 4] > 1) || ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i], i) == notForbidden) 
					three++;
				}

				//?�ա�ա���?
				if ((adjempty[i + 4] == 1) && (jumpsame[i + 4] == 2) && ((jumpempty[i + 4] > 1) || ((jumpempty[i + 4] == 1) && (jumpjumpsame[i + 4] == 0))) && ((adjempty[i] > 1) || ((adjempty[i] == 1) && (jumpsame[i + 4] == 0)))) {
					//if (KeyPointForbiddenCheck(x, y, adjsame[i + 4], i + 4) == notForbidden) 
					three++;
				}
			}
		}
		//�������������жϽ�����������

		if (four > 1) return doubleFour;
		if (three > 1)return doubleThree;
		return notForbidden;
	}
	// ����
	bool placePiece(int row, int col, state player) {
		if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != state::empty||((player==blackc)&&((ForbiddenCheck(row, col) != notForbidden)|| (ForbiddenCheck(row, col) != blackwin)))) {
			return false;
		}
		board[row][col] = player;
		return true;
	}
	// ��ȡ����״̬
	state getState(int row, int col) const {
		return board[row][col];
	}
	//��ֱ�Ӹ���֮ǰ��
	bool Won(state color) {
		//����
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
		//����
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
		//��������
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
		//��������
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

	// ��������Ƿ�����
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
//���ؿ���
int M(Board board,state player) {
	bool turn = 0;//0 black��1 white
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
	std::mt19937 gen(rd());//α�������������
	std::uniform_int_distribution<> dis(0, emptyCells.size() - 1);//ָ����Χ������Ǹ���

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
	ֻ�ж�ʤ������̫����
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

	//����
	//����
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
	//��
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

	//����
	//����
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
	//��
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

	//��������
	//����
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
	//��
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

	//��������
	//����
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
	//��
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

// alpha-beta ��֦�ļ�С�����㷨
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

//����
void SAVE() {
	std::ofstream outFile("memory.txt");
	if (!outFile.is_open()) {
		std::cerr << "�޷����ļ��洢��֣�" << std::endl;
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

	std::cout << "�ѳɹ��洢���~" << std::endl;
	return;
}

//��ȡ
void READ() {
	std::ifstream inFile("memory.txt");
	if (!inFile.is_open()) {
		std::cerr << "�޷����ļ���ȡ��֣�" << std::endl;
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

	std::cout << "�ѳɹ���ȡ���~" << std::endl;
	return;
}

//͸��ͼƬ
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg) {
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER,0,255,AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

//��ť��ûд��������Ϊ�ڼ���˵㲻���������⣩
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
//��ʼ��
void inb(Button& button,int _x, int _y, int _width, int _height) {
	button.x = _x;
	button.y = _y;
	button.width = _width;
	button.height = _height;
}//initialize button
//����
void drawbutton(Button button) {
	if (button.isHover) {
		putimage(button.x, button.y, &button.image_change);
	}
	else {
		putimage(button.x, button.y, &button.image);
	}
}
//����͸������
void transdrawbutton(Button button) {
	if (button.isHover) {
		transparentimage(NULL, button.x, button.y, &button.image_change);
	}
	else {
		transparentimage(NULL, button.x, button.y, &button.image);
	}
}
//����Ƿ��ڰ�ť��
void checkHover(Button* button,int mouseX, int mouseY) {
	button->isHover = ((mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= (button->y)) && (mouseY <= (button->y) + (button->height)));
}
//����Ƥ����ͼ����Ҫ����һ��ƫ��
void JX_checkHover(Button* button, int mouseX, int mouseY,int dy) {
	button->isHover = ((mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= ((button->y)+dy)) && (mouseY <= (button->y) + (button->height)));
}
//�Ƿ�����ť
void checkClick(Button* button,int mouseX, int mouseY) {
	button->isClicked = ((button->isHover) && (mouseX >= (button->x)) && (mouseX <= (button->x) + (button->width)) && (mouseY >= (button->y)) && (mouseY <= (button->y) + (button->height)));
}

//����Ƥ����ҳ��
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
		//��ͼ����
		if (lanother_state) {
			time++;
			time %= 50;//��ֹ̫��
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
			time++;//�볡����
			time %= 8;
			if (lx <= 11) {
				lanother_state = true;
			}
			else if (time == 0) {
				lx -= 10;
				ly += 1;
			}
		}
		//��ͼ����
		if (ranother_state) {
			if (time == 0) {
				add += 5;
				rx = 534 - 5 * sin(add);
				ry = 660 + 5 * cos(add);
			}
		}
		else {//�볡����
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
		//�볡�����Ƿ����
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
				PAGE = ftready;//�����ʼ����׼������
			}
			else if (setting.isClicked) {
				std::cout << "setting clicked" << std::endl;
				setting.isHover = false;
				PAGE = ftSetting;//������ý��뻻������
			}
			else if (rule.isClicked) {
				std::cout << "rule clicked" << std::endl;
				rule.isHover = false;
				PAGE = ftRule;//����������������
			}
			else std::cout << "nothing happend" << std::endl;
			break;
		}
	}
};
//����Ƥ��׼������ҳ��
class FeiTianReady {
private:
	IMAGE blur, lb;
	IMAGE n, nc, s, sc;
	Button newstart, save;
	int pagestep = 1;//��ʾҳ��
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
		if (pagestep == 2) {//�ڶ�ҳ��ѡģʽ
			transparentimage(NULL, 0, 0, &blur);
			drawbutton(pvp);
			drawbutton(pvc);
		}
		if (pagestep == 3) {//����ҳ���˻���ս��ѡ�Ⱥ���
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
					step = 0;//�µĿ�ʼ
					pagestep = 2;//��һҳ
					newstart.isHover = false;
					std::cout << "newstart clicked" << std::endl;
				}
				else if (save.isClicked) {
					READ();//��ȡ�浵
					pagestep = 2;
					save.isHover = false;
					std::cout << "read clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					std::cout << "return" << std::endl;
					PAGE = ftStart;//������ҳ��
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
					computer = false;//�������˹�����
					PAGE = ftGame;
					pagestep = 1;//��ʼ��Ϸ��page��ʼ��
					pvp.isHover = false;
					std::cout << "pvp clicked" << std::endl;
				}
				else if (pvc.isClicked) {
					computer = true;//�����˹�����
					pagestep = 3;//��һҳ
					pvc.isHover = false;
					std::cout << "pvc clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep = 1;//������һҳ
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
					XianShou = true;//����
					PAGE = ftGame;
					pagestep = 1;//��ʼ��
					first.isHover = false;
					std::cout << "first clicked" << std::endl;
				}
				else if (last.isClicked) {
					XianShou = false;//����
					PAGE = ftGame;
					pagestep = 1;//��ʼ��
					pvc.isHover = false;
					std::cout << "last clicked" << std::endl;
				}
				else if (msg.x <= 150 || msg.x >= 750 || msg.y <= 150 || msg.y >= 750) {
					pagestep = 2;//������һҳ
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
//����Ƥ������ҳ��
class FeiTianRule {
private:
	IMAGE blur;
	IMAGE r1, r2, r3, r4;
	IMAGE n, nc, p, pc;
	Button next, previous;
	int page = 1;//ҳ��
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
		//��ͬҳ��ʾ��ͬ��ť
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
		//��ͬҳ��ⲻͬ��ť
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
				page++;//��һҳ
				previous.isHover = false;
				next.isHover = false;
			}
			else if (previous.isClicked) {
				std::cout << "previous clicked" << std::endl;
				page--;//��һҳ
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
		//��ֹҳ������Χ
		if (page < 1) {
			page = 1;
		}
		if (page > 4) {
			page = 4;
		}
	}
};
//����Ƥ�����ã�����������
class FeiTianSetting {//�����ͼ�ͻỻƤ����û�����ʲô����������ݣ�
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
//����Ƥ���������
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
		//������
		if (step > 0) {
			int x = 0;
			int y = 0;
			//�������������ʤ��ԭ��
			if (end) {
				setfillcolor(RGB(139, 0, 18));//����죡����
				setlinecolor(RGB(139, 0, 18));
				for (int i = 0; i < 5; ++i) {
					x = 175 + winx[i] * 550 / 14;
					y = 175 + winy[i] * 550 / 14;
					fillcircle(x, y, R + 2);
				}
			}
			//������һ������������
			x = 175 + placex[step] * 550 / 14;
			y = 175 + placey[step] * 550 / 14;
			setfillcolor(RGB(114, 214, 191));
			setlinecolor(RGB(114, 214, 191));
			fillcircle(x, y, R+1);
		}
		//���ݵ�ǰ��ֻ�ͼ
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
	//����
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
		//��������ж��Ƿ�ʤ��
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
		//���Ų����������λ�û���ʾ����
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
			//�ж��Ƿ�������
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
			//�ж�ʱ������ͣ
			else if (stop.isClicked) {
				std::cout << "stop clicked" << std::endl;
				PAGE = ftstop;
				stop.isHover = false;
			}
			//�Ƿֳ�ʤ��ʱ����������λ��
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
										std::cout << "���Ľ���" << std::endl;
										MessageBox(NULL, "���Ľ���", "��������", MB_OK);
										break;
									}
									else if (con == doubleThree) {
										std::cout << "��������" << std::endl;
										MessageBox(NULL, "��������", "��������", MB_OK);
										break;
									}
									else if (con == longConnection) {
										std::cout << "��������" << std::endl;
										MessageBox(NULL, "��������", "��������", MB_OK);
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
			MessageBox(NULL, "ƽ��", "���", MB_OK);
		}
		turn = step % 2;//ͨ������ȷ��˭���ӣ�֮ǰ�Թ�turn=!turn�����ǻ�������⣩
		if (computer) {
			turn = step % 2;
			if (XianShou) {//�����֣����԰���
				computercolor = whitec;
				if ((turn % 2 == 1)&&(!end)) {//�ֵ���������
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
			else {//�˺��֣����Ժ���
				computercolor = blackc;
				if ((turn % 2 == 0)&&(!end)) {//�ֵ���������
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
//����Ƥ��������ͣ����
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
				step = 0;//����
				PAGE = ftGame;
				retry.isHover = false;
			}
			else if (save.isClicked) {
				SAVE();//����
				save.isHover = false;
				std::cout << "save clicked" << std::endl;
			}
			else if (exit.isClicked) {
				PAGE = ftStart;//����
				step = 0;//���ã�û�㱣����˳��������κ����ι���
				exit.isHover = false;
				std::cout << "exit clicked" << std::endl;
			}
			else if (costume.isClicked) {
				std::cout << "costume clicked" << std::endl;
				PAGE = jxstop;//��Ƥ��
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

//�ڶ���Ƥ���߼�ͬ�ϣ�������ע�ͣ���Ϊ�������Ǹ���ճ��0
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
		//ʹ�����й���Ч��
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
					computer = false;//�������˹�����
					PAGE = jxGame;
					pagestep = 1;
					pvp.isHover = false;
					std::cout << "pvp clicked" << std::endl;
				}
				else if (pvc.isClicked) {
					computer = true;//�����˹�����
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
					XianShou = true;//����
					PAGE = jxGame;
					pagestep = 1;
					first.isHover = false;
					std::cout << "first clicked" << std::endl;
				}
				else if (last.isClicked) {
					XianShou = false;//����
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
		//��������ж��Ƿ�ʤ��
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
		//���Ų����������λ�û���ʾ����
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
			//�ж��Ƿ�������
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
			//�ж�ʱ������ͣ
			else if (stop.isClicked) {
				std::cout << "stop clicked" << std::endl;
				PAGE = jxstop;
				stop.isHover = false;
			}
			//�Ƿֳ�ʤ��ʱ����������λ��
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
										std::cout << "���Ľ���" << std::endl;
										MessageBox(NULL, "���Ľ���", "��������", MB_OK);
										break;
									}
									else if (con == doubleThree) {
										std::cout << "��������" << std::endl;
										MessageBox(NULL, "��������", "��������", MB_OK);
										break;
									}
									else if (con == longConnection) {
										std::cout << "��������" << std::endl;
										MessageBox(NULL, "��������", "��������", MB_OK);
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
			MessageBox(NULL, "ƽ��", "���", MB_OK);
		}
		turn = step % 2;//ͨ������ȷ��˭���ӣ�֮ǰ�Թ�turn=!turn�����ǻ�������⣩
		if (computer) {
			turn = step % 2;
			if (XianShou) {//�����֣����԰���
				computercolor = whitec;
				if ((turn % 2 == 1) && (!end)) {//�ֵ���������
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
			else {//�˺��֣����Ժ���
				computercolor = blackc;
				if ((turn % 2 == 0) && (!end)) {//�ֵ���������
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

//����
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
		//˫�����ͼ����Ҫ���ڻ�ͼ����֮ǰ��֮��
		BeginBatchDraw();//��ʼ������ͼ

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

		FlushBatchDraw();//���������ͼ

	}

	closegraph();
	PostQuitMessage(0);
}

int main() {

	initgraph(900, 900, EW_SHOWCONSOLE);

	HWND hnd = GetHWnd();
	SetWindowText(hnd, "������");

	run();
	
	return 0;
}
