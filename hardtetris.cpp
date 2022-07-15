#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <conio.h>
using namespace std;

void consoleInit()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x*2;
	coord.Y = y;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, coord);
}

const int WIDTH = 10;
const int HEIGHT = 24;
const string noChar = "  ";
const string tetrionChar = "¨€";
const string shapeChar = "¡õ";

const int shape[7][4][4] =
	{
		{{0, 0, 0, 0},
		 {1, 1, 1, 1},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{1, 0, 0},
		 {1, 1, 1},
		 {0, 0, 0}},
		{{0, 0, 1},
		 {1, 1, 1},
		 {0, 0, 0}},
		{{1, 1},
		 {1, 1}},
		{{0, 1, 1},
		 {1, 1, 0},
		 {0, 0, 0}},
		{{0, 1, 0},
		 {1, 1, 1},
		 {0, 0, 0}},
		{{1, 1, 0},
		 {0, 1, 1},
		 {0, 0, 0}}};

const int emptyShape[4][4] = {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};

const int size[7] = {4, 3, 3, 2, 3, 3, 3};

void drawAt(int x, int y, string c)
{
	gotoxy(x, y);
	cout << c;
}

void rotateShape(int a[4][4], int no, int n = 1)
{
	int b[4][4];
	for (int i = 1; i <= n; i++)
	{
		for (int x = 0; x < size[no]; x++)
			for (int y = 0; y < size[no]; y++)
				b[x][size[no] - 1 - y] = a[y][x];
		for (int x = 0; x < size[no]; x++)
			for (int y = 0; y < size[no]; y++)
				a[x][y] = b[x][y];
	}
}

void copyShape(const int a[4][4], int b[4][4])
{
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
			b[y][x] = a[y][x];
}

void drawTetrion()
{
	for (int x = 0; x <= WIDTH + 1; x++)
		drawAt(x, 0, tetrionChar);
	for (int x = 0; x <= WIDTH + 1; x++)
		drawAt(x, HEIGHT + 1, tetrionChar);
	for (int y = 1; y <= HEIGHT + 1; y++)
		drawAt(0, y, tetrionChar);
	for (int y = 1; y <= HEIGHT + 1; y++)
		drawAt(WIDTH + 1, y, tetrionChar);
}

void drawShape(int x, int y, const int sh[4][4], bool erase = false)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (sh[j][i])
				drawAt(x + i, y + j, (erase ? noChar : shapeChar));
}

bool board[HEIGHT + 5][WIDTH + 5];
int tx, ty, tNo, tNextNo, tShape[4][4], score, lvl, lineCnt;
const int scoreTable[5] = {0, 40, 300, 1200, 3000};

int spawnx(int no)
{
	return WIDTH / 2 - (size[no] + 1) / 2 + 1;
}

int spawny(int no)
{
	return (lvl >= 5 ? 2 : 1);
}

bool valid(int x, int y, int sh[4][4])
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (sh[j][i])
			{
				if (y + j > HEIGHT)
					return false;
				if (x + i < 1 || x + i > WIDTH)
					return false;
				if (board[y + j][x + i])
					return false;
			}
	return true;
}

void toBoard(int x, int y, int sh[4][4])
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (sh[j][i])
				board[y + j][x + i] = true;
}

void printBoard()
{
	for (int x = 1; x <= WIDTH; x++)
		for (int y = 1; y <= HEIGHT; y++)
		{
			if (board[y][x])
				drawAt(x, y, shapeChar);
			else
				drawAt(x, y, noChar);
		}
}

void elimLines()
{
	vector<int> fullLines;
	for (int row = 1; row <= HEIGHT; row++)
	{
		bool full = true;
		for (int i = 1; i <= WIDTH; i++)
			if (!board[row][i])
				full = false;
		if (full)
			fullLines.push_back(row);
	}
	int nLines = fullLines.size();
	lineCnt += nLines;
	lvl = lineCnt / 10 + 1;
	score += scoreTable[nLines] * lvl;
	for (int i = 1; i <= WIDTH / 2; i++)
	{
		for (int rowi = 0; rowi < nLines; rowi++)
		{
			int y = fullLines[rowi];
			drawAt(WIDTH / 2 - i + 1, y, noChar);
			drawAt(WIDTH / 2 + i, y, noChar);
		}
		Sleep(70);
	}
	for (int rowi = 0; rowi < nLines; rowi++)
	{
		int row = fullLines[rowi];
		for (int i = row; i >= 1; i--)
			for (int j = 1; j <= WIDTH; j++)
				board[i][j] = board[i - 1][j];
	}
	printBoard();
}

void init()
{
	memset(board, 0, sizeof(board));
	drawTetrion();
	printBoard();
	srand(time(0));
	tNextNo = rand() % 7;
	lvl = 1;
	score=0;
}

void spawn()
{
	tNo = tNextNo;
	tNextNo = rand() % 7;
	copyShape(shape[tNo], tShape);
	tx = spawnx(tNo);
	ty = spawny(tNo);
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
	rotateShape(tShape, tNo);
	if (valid(tx, ty, tShape))
		;
	else if (valid(tx - 1, ty, tShape))
		tx--;
	else if (valid(tx + 1, ty, tShape))
		tx++;
	else
		rotateShape(tShape, tNo, 3);
	drawShape(tx, ty, tShape);
}

void info()
{
	gotoxy(WIDTH + 4, 3);
	cout << "PRESS P TO PAUSE" << endl;
	gotoxy(WIDTH + 4, 4);
	cout << "NUMBER OF ROWS ELIMINATED£º" << lineCnt << "              " << endl;
	gotoxy(WIDTH + 4, 5);
	cout << "DIFFICULTY£º" << lvl     << "              " << endl;
	gotoxy(WIDTH + 4, 6);
	cout << "SCORE£º" << score   << "              " << endl;
	gotoxy(WIDTH + 4, 7);
	cout << "NEXT:" << endl;
	drawShape(WIDTH + 6, 9, emptyShape, true);
	drawShape(WIDTH + 6, 9, shape[tNextNo]);
}

void pause()
{
	gotoxy(WIDTH+4,3);
	cout << "PRESS P TO CONTINUE" << endl;
	while (true)
		if (kbhit() && getch() == 'p')
			break;
}

bool keyPress(int key)
{
	SHORT state = GetAsyncKeyState(key);
	return (1 << 15) & state;
}

void game()
{
	int downClock;
	bool downKey;
	init();
	spawn();
	drawShape(tx, ty, tShape);
	info();
	downClock = clock();
	while (true)
	{
		downKey = false;
		gotoxy(0, HEIGHT + 2);
		if (keyPress(VK_DOWN))
			downKey = true;
		if (kbhit())
		{
			int k = getch();
			if (k == 'p')
				pause();
			else if (k == 224)
			{
				int k = getch();
				if (k == 72)
					rotate();
				else if (k == 75)
					left();
				else if (k == 77)
					right();
			}
		}
		int timing=max(500/lvl,50);
		if(downKey)timing=min(timing,70);
		if (clock() - downClock >= timing)
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
				downClock = clock();
				continue;
			}
			drawShape(tx, ty, tShape, 1);
			ty++;
			drawShape(tx, ty, tShape);
			downClock = clock();
		}
	}
}

int main()
{
	consoleInit();
	system("pause");
	while (1)
	{
		game();
		gotoxy(0, HEIGHT + 2);
		cout << "GAME OVER" << endl;
		cout << "SCORE: " << score << endl;
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
