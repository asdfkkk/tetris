#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <conio.h>
using namespace std;

HANDLE hConsole;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 100;
        info.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &info);
        exit(0);
    }
    return TRUE;
}

void consoleInit()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
    SetConsoleCtrlHandler(consoleHandler, TRUE);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x * 2;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

const int WIDTH = 10;
const int HEIGHT = 24;
const string noChar = "1";
const string tetrionChar = "2";
const string shapeChar = "3";

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
    if (c == "1")
    {
        SetConsoleTextAttribute(hConsole, 0);
    }
    else if (c == "2")
    {
        SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    }
    else
    {
        SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE);
    }
    cout << "  ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
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

void drawAtColored(int x, int y, int color)
{
    SetConsoleTextAttribute(hConsole, color);
    gotoxy(x, y);
    cout << "  ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

void drawShape(int x, int y, const int sh[4][4], bool erase = false, int color = BACKGROUND_BLUE)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (sh[j][i])
                drawAtColored(x + i, y + j, (erase ? 0 : color));
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
    score = 0;
}

void spawn()
{
    tNo = tNextNo;
    tNextNo = rand() % 7;
    copyShape(shape[tNo], tShape);
    tx = spawnx(tNo);
    ty = spawny(tNo);
}

void drawLimit()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    for (int x = 1; x <= WIDTH; x++)
    {
        if (board[4][x])
        {
            continue;
        }
        gotoxy(x, 4);
        cout << "__";
    }
}

void left()
{
    if (valid(tx - 1, ty, tShape))
    {
        drawShape(tx, ty, tShape, true);
        tx--;
        drawLimit();
        drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
    }
}

void right()
{
    if (valid(tx + 1, ty, tShape))
    {
        drawShape(tx, ty, tShape, true);
        tx++;
        drawLimit();
        drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
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
    drawLimit();
    drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
}

void info()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    gotoxy(WIDTH + 4, 2);
    cout << "Press Ctrl-C to quit." << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    gotoxy(WIDTH + 4, 3);
    cout << "PRESS P TO PAUSE" << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    gotoxy(WIDTH + 4, 4);
    cout << "NUMBER OF ROWS ELIMINATED: " << lineCnt << "              " << endl;
    gotoxy(WIDTH + 4, 5);
    cout << "DIFFICULTY: " << lvl << "              " << endl;
    gotoxy(WIDTH + 4, 6);
    cout << "SCORE: " << score << "              " << endl;
    gotoxy(WIDTH + 4, 7);
    cout << "NEXT: " << endl;
    drawShape(WIDTH + 6, 9, emptyShape, true);
    drawShape(WIDTH + 6, 9, shape[tNextNo], 0, BACKGROUND_GREEN);
}

void pause()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    gotoxy(WIDTH + 4, 3);
    cout << "PRESS P TO CONTINUE" << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    while (true)
        if (kbhit() && getch() == 'p')
            break;
    gotoxy(WIDTH + 4, 3);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout << "PRESS P TO PAUSE      " << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
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
    drawLimit();
    drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
    info();
    downClock = clock();
    while (true)
    {
        downKey = false;
        gotoxy(0, HEIGHT + 2);
        if (keyPress(0x53) || keyPress(VK_DOWN))
            downKey = true;
        if (kbhit())
        {
            int k = getch();
            if (k == 'p')
                pause();
            else if (k == 'a')
                left();
            else if (k == 'd')
                right();
            else if (k == 'w')
                rotate();
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
        int timing = max(500 / lvl, 50);
        if (downKey)
            timing = min(timing, 70);
        if (clock() - downClock >= timing)
        {
            if (!valid(tx, ty + 1, tShape))
            {
                bool fail = false;
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (tShape[j][i] && ty + j <= 4)
                        {
                            fail = true;
                        }
                    }
                }
                toBoard(tx, ty, tShape);
                printBoard();
                drawLimit();
                if (fail)
                    break;
                elimLines();
                info();
                drawLimit();
                spawn();
                if (!valid(tx, ty, tShape))
                    break;
                info();
                drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
                downClock = clock();
                continue;
            }
            drawLimit();
            drawShape(tx, ty, tShape, 1);
            ty++;
            drawLimit();
            drawShape(tx, ty, tShape, 0, BACKGROUND_RED);
            downClock = clock();
        }
    }
}

int main()
{
    system("title TETRIS");
    consoleInit();
    cout << "press any key to start";
    while (true)
        if (kbhit())
        {
            getch();
            break;
        }
    while (1)
    {
        system("cls");
        game();
        gotoxy(0, HEIGHT + 2);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        cout << "GAME OVER" << endl;
        cout << "SCORE: " << score << endl;
        cout << "PRESS R TO RESTART" << endl;
        while (true)
            if (kbhit() && getch() == 'r')
                break;
        gotoxy(0, HEIGHT + 2);
        cout << "                     " << endl;
        cout << "                     " << endl;
        cout << "                     " << endl;
    }
    return 0;
}
