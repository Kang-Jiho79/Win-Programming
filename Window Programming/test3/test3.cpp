//실습 12번
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 25
#define cellsize 25

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// 메인함수 
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;	//윈도우 클래스
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);	//	구조체의 크기
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//	윈도우 출력 스타일
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//	윈도우 메세지를 처리하는 윈도우 프로시저 함수 이름 (Long Pointer Funtion WndProc)
	WndClass.cbClsExtra = 0;	//	사용여분 메모리(class)
	WndClass.cbWndExtra = 0;	//	사용여분 메모리(Window)
	WndClass.hInstance = hInstance;		//응용 프로그램 ID
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// 실행파일의 쓰일 아이콘지정
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// 윈도우에서 쓰일 커서지정
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 윈도우의 배경색을 설정할수있다.
	WndClass.lpszMenuName = NULL;	//메뉴 이름
	WndClass.lpszClassName = IpszClass;		//	클래스의 이름
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//작은 아이콘 (보통 hIcon과 같은걸 사용)
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1100, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

	ShowWindow(hWnd, nCMdShow);	//	윈도우 띄우기
	UpdateWindow(hWnd);		// 윈도우 업데이트

	while (GetMessage(&Message, 0, 0, 0)) {		//	메시지 루프 (메시지 받아오는 역할) -  종료메시지가 들어올때까지 반복
		TranslateMessage(&Message);		//	메시지 번역
		DispatchMessage(&Message);	//	메시지 처리
	}
	return Message.wParam;
}

struct Player {
	int x, y;
	COLORREF color;
	int size; 
	int shape;
	int shapeTimer;
};

struct Cell {
	int type; // 0: 일반, 1: 장애물, 2: 색상 변경, 3: 축소, 4: 확대, 5: 모양 변경, 6: 목표 지점
	COLORREF color;
};

Player player[2];
int presentplayer = 0;
Cell table[tablecount][tablecount];
int goalsize;
int goalshape;
COLORREF goalcolor;
POINT goal;
int ending = 2;

COLORREF red = RGB(255, 0, 0);	//장애물 색
COLORREF green = RGB(0, 255, 0); //축소 색
COLORREF blue = RGB(0, 0, 255); //확대 색
COLORREF black = RGB(0, 0, 0); //모양변경 색
COLORREF colors[5];

// 게임 기초 세팅
void settinggame()
{
	srand((unsigned)time(0));
	ending = 2;
	for (int i = 0; i < tablecount; ++i) {
		for (int j = 0; j < tablecount; ++j) {
			table[i][j].type = 0;
			table[i][j].color = RGB(255, 255, 255);
		}
	}
	player[0] = { 0, 0, RGB(rand() % 256, rand() % 256, rand() % 256), cellsize/4, 0, 0 };
	player[1] = { tablecount - 1, 0, RGB(rand() % 256, rand() % 256, rand() % 256), cellsize/4, 0, 0 };

	for (int i = 0; i < 5; i++) {
		colors[i] = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
	goalcolor = colors[rand() % 5];
	goalshape = rand() % 4 + 1;
	goal = { tablecount / 2,tablecount - 1 };
	table[goal.y][goal.x].type = 6;
	goalsize = (rand() % 4 + 1) * cellsize/4;

	for (int i = 0; i < 20; ++i) {
		int x = rand() % tablecount;
		int y = rand() % tablecount;
		if (table[y][x].type == 0) {
			table[y][x].type = 1;
			table[y][x].color = red;
		}
		else
			i--;
	}
	for (int i = 0; i < 20; ++i) {
		int x = rand() % tablecount;
		int y = rand() % tablecount;
		if (table[y][x].type == 0) {
			table[y][x].type = 2;
			table[y][x].color = colors[rand() % 5];
		}
		else
			i--;
	}
	for (int i = 0; i < 20; ++i) {
		int x = rand() % tablecount;
		int y = rand() % tablecount;
		if (table[y][x].type == 0) {
			table[y][x].type = 3;
			table[y][x].color = green;
		}
		else
			i--;
	}
	for (int i = 0; i < 20; ++i) {
		int x = rand() % tablecount;
		int y = rand() % tablecount;
		if (table[y][x].type == 0) {
			table[y][x].type = 4;
			table[y][x].color = blue;
		}
		else
			i--;
	}
	for (int i = 0; i < 20; ++i) {
		int x = rand() % tablecount;
		int y = rand() % tablecount;
		if (table[y][x].type == 0) {
			table[y][x].type = 5;
			table[y][x].color = black;
		}
		else
			i--;
	}
	table[goal.x - 1][goal.y-1].type = 2;
	table[goal.x - 1][goal.y-1].color = goalcolor;
}

// 40 x 40 바닥칠하기
void floor(HDC hdc, int x, int y)
{
	int rx = cellsize / 2 + (cellsize * x);
	int ry = cellsize / 2 + (cellsize * y);
	Rectangle(hdc, rx - cellsize / 2, ry - cellsize / 2, rx + cellsize / 2, ry + cellsize / 2);
}

// 40 x 40 윗화살표 그리기
void uparrow(HDC hdc, int x, int y)
{
	int rx = cellsize/2 + (cellsize * x);
	int ry = cellsize / 2 + (cellsize  * y);
	POINT point[8] = { {rx,ry - cellsize / 2},{rx - cellsize / 2,ry},{rx - cellsize / 4,ry},{rx - cellsize / 4,ry + cellsize / 2},{rx + cellsize / 4,ry + cellsize / 2},{rx + cellsize / 4,ry},{rx + cellsize / 2,ry},{rx,ry - cellsize / 2} };
	Polygon(hdc, point, 8);
}

// 40 x 40 아래화살표 그리기
void downarrow(HDC hdc, int x, int y)
{
	int rx = cellsize / 2 + (cellsize * x);
	int ry = cellsize / 2 + (cellsize * y);
	POINT point[8] = { {rx,ry + cellsize / 2},{rx - cellsize / 2,ry},{rx - cellsize / 4,ry},{rx - cellsize / 4,ry - cellsize / 2},{rx + cellsize / 4,ry - cellsize / 2},{rx + cellsize / 4,ry},{rx + cellsize / 2,ry},{rx,ry + cellsize / 2} };
	Polygon(hdc, point, 8);
}

void drawtable(HDC hdc)
{
	for (int y = 0; y < tablecount; y++) {
		for (int x = 0; x < tablecount; x++) {
			HBRUSH brush = CreateSolidBrush(table[y][x].color);
			SelectObject(hdc, brush);
			floor(hdc, x, y);
			DeleteObject(brush);
			if (table[y][x].type == 3)
				downarrow(hdc, x, y);
			else if (table[y][x].type == 4)
				uparrow(hdc, x, y);
		}
	}
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}

void MovePlayer(int dx, int dy) {
	Player& p = player[presentplayer];
	int nx = p.x + dx;
	int ny = p.y + dy;
	if (nx < 0 || ny < 0 || nx >= tablecount || ny >= tablecount)
		return;
	if (table[ny][nx].type == 1)
		return;

	p.x = nx;
	p.y = ny;

	Cell& cell = table[ny][nx];
	switch (cell.type) {
	case 2: p.color = cell.color; break;
	case 3: p.size = (p.size > cellsize / 4) ? p.size - cellsize / 4 : cellsize; break;
	case 4: p.size = (p.size < cellsize) ? p.size + cellsize / 4 : cellsize / 4; break;
	case 5:
		p.shape = rand() % 4 + 1;
		p.shapeTimer = 10;
		break;
	}

	if (p.shapeTimer > 0) {
		--p.shapeTimer;
		if (p.shapeTimer == 0)
			p.shape = 0; // 되돌리는 대신 랜덤으로
	}

	if (p.x == goal.x && p.y == goal.y && p.shape == goalshape && p.color == goalcolor && p.size == goalsize) {
		if (presentplayer == 0)
			ending = 0;
		else if (presentplayer == 1)
			ending = 1;
	}

	presentplayer = 1 - presentplayer;
}


// 원 그리기
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size/2, y - size / 2, x + size / 2, y + size / 2);
}
// 타원 그리기
void oval(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 3, x + size / 2, y + size / 3);
}
// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}
// 삼각형 그리기
void triangle(HDC hdc, int x, int y, int size)
{
	POINT point[3] = { {x,y - size * 2/3}, {x - size / 2, y + size /3},{x + size / 2, y + size / 3} };
	Polygon(hdc, point, 3);
}
// 모레시계 그리기 player 1
void sandglass(HDC hdc, int x, int y, int size)
{
	POINT point[6] = { {x,y},{x - size / 2,y - size / 2},{x + size / 2,y - size / 2},{x,y},{x + size / 2,y + size / 2},{x - size / 2,y + size / 2}};
	Polygon(hdc, point, 6);
}

// 오각형 그리기 player 2
void pentagon(HDC hdc, int x, int y, int size)
{
	double pi = 3.14;
	int r = size * 3 / 5;
	POINT point[5];
	for (int i = 0; i < 5; i++) {
		double angle = -pi / 2 + i * (2 * pi / 5);
		point[i].x = (x + r * cos(angle));
		point[i].y = (y + r * sin(angle));
	}
	Polygon(hdc, point, 5);
}

void drawshape(HDC hdc, int x, int y, COLORREF color, int size, int shape, int now)
{
	int rx = cellsize/2 + (cellsize * x);
	int ry = cellsize/2 + (cellsize * y);
	HBRUSH brush = CreateSolidBrush(color);
	SelectObject(hdc, brush);
	switch (shape)
	{
	case 0:
		if(!now)
			sandglass(hdc, rx, ry, size);
		else
			pentagon(hdc, rx, ry, size);
		break;
	case 1:
		triangle(hdc, rx, ry, size);
		break;
	case 2:
		square(hdc, rx, ry, size);
		break;
	case 3:
		circle(hdc, rx, ry, size);
		break;
	case 4:
		oval(hdc, rx, ry, size);
		break;
	default:
		break;
	}
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HPEN hDefPen;
	HBRUSH hDefBrush, hRedBrush, hBlueBrush, hGreenBrush, hYellowBrush, hTurquoiseBrush;
	RECT Now;
	switch (iMessage) {
	case WM_CREATE:
		settinggame();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		drawtable(hdc);

		for (int i = 0; i < 2; ++i)
			drawshape(hdc, player[i].x, player[i].y, player[i].color, player[i].size, player[i].shape, i);

		drawshape(hdc, goal.x, goal.y, goalcolor, goalsize, goalshape, 0);
		if (ending == 0)
			TextOut(hdc, 0, cellsize * tablecount, L"player1 승리", 10);
		else if(ending == 1)
			TextOut(hdc, 0, cellsize * tablecount, L"player2 승리", 10);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (ending == 2) {
			if (wParam == 'Q')
				PostQuitMessage(0);
			else {
				if (presentplayer == 0) {
					if (wParam == 'W') MovePlayer(0, -1);
					else if (wParam == 'A') MovePlayer(-1, 0);
					else if (wParam == 'S') MovePlayer(0, 1);
					else if (wParam == 'D') MovePlayer(1, 0);
				}
				else {
					if (wParam == 'I') MovePlayer(0, -1);
					else if (wParam == 'J') MovePlayer(-1, 0);
					else if (wParam == 'K') MovePlayer(0, 1);
					else if (wParam == 'L') MovePlayer(1, 0);
				}
			}
		}
		else if (wParam == 'R')
			settinggame();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
return 0;
}
