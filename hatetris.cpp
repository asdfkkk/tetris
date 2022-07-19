#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <conio.h>
using namespace std;

HANDLE hConsole;

void consoleInit()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &info);
}

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x*2;
	coord.Y = y;
	SetConsoleCursorPosition(hConsole, coord);
}

const int WIDTH = 10;
const int HEIGHT = 24;
const string noChar = "  ";
const string tetrionChar = "¨€";
const string shapeChar = "¡õ";

const int shape[7][4][4] =
	{
		// I
		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0}},
		// J
		{{0, 0, 1, 0},
		 {0, 0, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		// L
		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		// O
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		// S 
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {1, 1, 0, 0},
		 {0, 0, 0, 0}},
		// Z
		{{0, 0, 0, 0},
		 {1, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		// T
		{{0, 0, 0, 0},
		 {0, 1, 1, 1},
		 {0, 0, 1, 0},
		 {0, 0, 0, 0}},
	};


void drawAt(int x, int y, string c)
{
	gotoxy(x, y);
	cout << c;
}

void copyShape(const int a[4][4], int b[4][4])
{
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
			b[y][x] = a[y][x];
}

void rotateShape(int a[4][4], int n = 1)
{
	int b[4][4];
	for (int i = 1; i <= n; i++)
	{
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++)
				b[x][3-y] = a[y][x];
		copyShape(b, a);
	}
}

void drawTetrion()
{
	SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED);
	for (int x = 0; x <= WIDTH + 1; x++)
	{
		gotoxy(x, 0);
		cout << "  ";
	}
	for (int x = 0; x <= WIDTH + 1; x++)
	{
		gotoxy(x, HEIGHT+1);
		cout << "  ";
	}
	for (int y = 1; y <= HEIGHT + 1; y++)
	{
		gotoxy(0, y);
		cout << "  ";
	}
	for (int y = 1; y <= HEIGHT + 1; y++)
	{
		gotoxy(WIDTH+1, y);
		cout << "  ";
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
}

void drawShape(int x, int y, const int sh[4][4], bool erase = false)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (sh[j][i] && (1<=x+i&&x+i<=WIDTH&&1<=y+j&&y+j<=HEIGHT))
				drawAt(x + i, y + j, (erase ? noChar : shapeChar));
}

bool board[HEIGHT+9][WIDTH+9], temp[HEIGHT+9][WIDTH+9];
int tx, ty, tNo, tNextNo, tShape[4][4], lineCnt;

bool valid(int x, int y, const int sh[4][4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (sh[j][i])
			{
				if (y + j > HEIGHT)
					return false;
				if (x + i < 1 || x + i > WIDTH)
					return false;
				if (board[y + j][x + i])
					return false;
			}
		}
	}
	return true;
}

void toBoard(int x, int y, const int sh[4][4])
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (sh[j][i])
				board[y+j][x+i] = true;
}

void printBoard()
{
	for (int x = 1; x <= WIDTH; x++)
		for (int y = 1; y <= HEIGHT; y++)
			drawAt(x, y, (board[y][x] ? shapeChar : noChar));
}

void elimLines(bool anim = true)
{
	vector<int> fullLines;
	for (int row = 1; row <= HEIGHT; row++)
	{
		bool full = true;
		for (int i = 1; i <= WIDTH; i++)
		{
			if (!board[row][i])
			{
				full = false;
				break;
			}
		}
		if (full)
		{
			fullLines.push_back(row);
		}
	}
	int nLines = fullLines.size();
	if (anim)
	{
		lineCnt += nLines;
		for (int i = 1; i <= WIDTH / 2; i++)
		{
			for (int iR = 0; iR < nLines; iR++)
			{
				drawAt(WIDTH/2-i+1, fullLines[iR], noChar);
				drawAt(WIDTH/2+i,   fullLines[iR], noChar);
			}
			Sleep(70);
		}
	}
	for (int iR = 0; iR < nLines; iR++)
	{
		for (int i = fullLines[iR]; i >= 1; i--)
			for (int j = 1; j <= WIDTH; j++)
				board[i][j] = board[i - 1][j];
	}
	if (anim)
	{
		printBoard();
	}
}

void init()
{
	memset(board, 0, sizeof(board));
	drawTetrion();
	printBoard();
	srand(time(0));
}

int findMinHeight(int no){
	for (int x = 1; x <= WIDTH; x++)
		for (int y = 1; y <= HEIGHT; y++)
			temp[y][x] = board[y][x];
	int sh[4][4];
	copyShape(shape[no], sh);
	int h = HEIGHT + 10;
	for (int i = 1; i <= 4; i++)
	{
		for (int x = 1; x <= WIDTH; x++)
		{
			if (!valid(x, -5, sh))
			{
				break;
			}
			int y = -1;
			for (; ; y++)
			{
				if (!valid(x, y, sh))
				{
					break;
				}
			}
			y--;
			toBoard(x, y, sh);
			elimLines(false);
			int row = 1;
			for (; ; row++)
			{
				bool empty = true;
				for (int i = 1; i <= WIDTH; i++)
				{
					if (board[row][i])
					{
						empty = false;
						break;
					}
				}
				if (!empty)
				{
					break;
				}
			}
			h = min(h, HEIGHT - row + 1);
			for (int x = 1; x <= WIDTH; x++)
				for (int y = 1; y <= HEIGHT; y++)
					board[y][x] = temp[y][x];
		}
		rotateShape(sh);
	}
	return h;
}

int findHardShape()
{
	int h = 0, tno;
	for(int no = 0; no < 7; no++)
	{
		if (findMinHeight(no) >= h)
		{
			h = findMinHeight(no);
			tno = no;
		}
	}
	return tno;
}

void spawn()
{
	tNo = findHardShape();
	copyShape(shape[tNo], tShape);
	tx = WIDTH/2-1;
	ty = -2;
}

void left()
{
	if (valid(tx - 1, ty, tShape))
	{
		drawShape(tx, ty, tShape, true);
		tx--;
		drawShape(tx, ty, tShape);
	}
}

void right()
{
	if (valid(tx + 1, ty, tShape))
	{
		drawShape(tx, ty, tShape, true);
		tx++;
		drawShape(tx, ty, tShape);
	}
}

void rotate()
{
	drawShape(tx, ty, tShape, true);
	rotateShape(tShape);
	if (valid(tx, ty, tShape))
		;
	else if (valid(tx - 1, ty, tShape))
		tx--;
	else if (valid(tx + 1, ty, tShape))
		tx++;
	else
		rotateShape(tShape, 3);
	drawShape(tx, ty, tShape);
}

void info()
{
	gotoxy(WIDTH + 4, 4);
	cout << "NUMBER OF ROWS ELIMINATED: " << lineCnt << "              ";
	gotoxy(WIDTH + 4, 5);
	gotoxy(WIDTH + 4, 6);
}

void game()
{
	init();
	spawn();
	drawShape(tx, ty, tShape);
	info();
	bool down;
	while (1)
	{
		down = false;
		if (kbhit() && getch() == 224)
		{
			switch (getch())
			{
				case 72: rotate(); break;
				case 75: left();   break;
				case 77: right();  break;
				case 80: down = true;
			}
		}
		gotoxy(0, HEIGHT + 2);
		if (down)
		{
			if (!valid(tx, ty + 1, tShape))
			{
				toBoard(tx, ty, tShape);
				elimLines();
				info();
				spawn();
				if (!valid(tx, ty, tShape))
					break;
				info();
				drawShape(tx, ty, tShape);
				continue;
			}
			drawShape(tx, ty, tShape, 1);
			ty++;
			drawShape(tx, ty, tShape);
		}
	}
}

int main()
{
	consoleInit();
	while (1)
	{
		game();
		gotoxy(0, HEIGHT + 2);
		cout << "GAME OVER" << endl;
		cout << "NUMBER OF ROWS ELIMINATED: " << lineCnt << endl;
		cout << "PRESS R TO RESTART" << endl;
		while (true)
			if (kbhit() && getch() == 'r')
				break;
		gotoxy(0, HEIGHT + 2);
		cout << "                       " << endl;
		cout << "                       " << endl;
		cout << "                       " << endl;
	}
	return 0;
}
