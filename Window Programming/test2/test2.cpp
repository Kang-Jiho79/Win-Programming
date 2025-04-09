//실습 12번
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 25
#define cellsize 25
#define maxcell 10

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


struct Cell {
	int shapetype;	// 0 : 비어있음 1 : 원 2 : 삼각형 3 ; 사각형
	int x, y;
	int size = cellsize;
	COLORREF color;
};

Cell player;
Cell cell[10];
int presentplayer = 0;
int cellcount = 0;

COLORREF colors[4];

bool circlecheck = false;
COLORREF circlecolor;
bool trianglecheck = false;
COLORREF trianglecolor;
bool squarecheck = false;
COLORREF squarecolor;

// 게임 기초 세팅
void settinggame()
{
	srand(unsigned(time(NULL)));
	player.shapetype = 0;
	presentplayer = 0;
	for (int i = 0; i < maxcell; i++) {
		cell[i].shapetype = 0;
	}
	cellcount = 0;
	for (int i = 0; i < 4; i++) {
		colors[i] = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
	circlecolor = RGB(rand() % 256, rand() % 256, rand() % 256);
	trianglecolor = RGB(rand() % 256, rand() % 256, rand() % 256);
	squarecolor = RGB(rand() % 256, rand() % 256, rand() % 256);
}

void drawtable(HDC hdc)
{
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}

void selectplayer(int i)
{
	if (cell[i].shapetype == 0)
		return;
	if (player.shapetype != 0) {
		cell[presentplayer] = player;
	}
	player = cell[i];
	presentplayer = i;
	cell[i].shapetype = 0;
}

void MovePlayer(int dx, int dy) {
	int nx = player.x + dx;
	int ny = player.y + dy;
	if (nx < 0) {
		nx = tablecount - 1;
	}
	else if (nx >= tablecount)
		nx = 1;
	if (ny < 0) {
		ny = tablecount - 1;
	}
	else if (ny >= tablecount)
		ny = 1;
	player.x = nx;
	player.y = ny;
}

void changecolor()
{
	switch (player.shapetype)
	{
	case 0:
		return;
	case 1:
		circlecheck = !circlecheck;
		break;
	case 2:
		trianglecheck = !trianglecheck;
		break;
	case 3:
		squarecheck = !squarecheck;
		break;
	default:
		break;
	}
}

void sizeup()
{
	if (player.shapetype == 0)
		return;
	if (player.size >= cellsize * 5)
		return;
	player.size += cellsize;
}

void sizedown()
{
	if (player.shapetype == 0)
		return;
	if (player.size <= cellsize)
		return;
	player.size -= cellsize;
}

void deletecell()
{
	player.shapetype = 0;
	for (int i = presentplayer; i < maxcell - 1; i++)
		cell[i] = cell[i + 1];
	presentplayer = 0;
}

// 원 그리기
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}
// 삼각형 그리기
void triangle(HDC hdc, int x, int y, int size)
{
	POINT point[3] = { {x,y - size * 2 / 3}, {x - size / 2, y + size / 3},{x + size / 2, y + size / 3} };
	Polygon(hdc, point, 3);
}


// 셀 설정
void selectcircle()
{
	if (cellcount == 10) {
		for (int i = 0; i < maxcell - 1; i++)
			cell[i] = cell[i + 1];
		cellcount = 9;
	}
	cell[cellcount].shapetype = 1;
	cell[cellcount].color = colors[rand() % 4];
	bool check = true;
	while (check)
	{
		check = false;
		cell[cellcount].x = rand() % tablecount;
		cell[cellcount].y = rand() % tablecount;
		for (int i = 0; i < cellcount; i++) {
			if (cell[cellcount].x == cell[i].x && cell[cellcount].y == cell[i].y)
				check = true;
		}
	}
	cell[cellcount].size = maxcell;
	cellcount++;
}

void selecttriangle()
{
	if (cellcount == 10) {
		for (int i = 0; i < maxcell - 1; i++)
			cell[i] = cell[i + 1];
		cellcount = 9;
	}
	cell[cellcount].shapetype = 2;
	cell[cellcount].color = colors[rand() % 4];
	bool check = true;
	while (check)
	{
		check = false;
		cell[cellcount].x = rand() % tablecount;
		cell[cellcount].y = rand() % tablecount;
		for (int i = 0; i < cellcount; i++) {
			if (cell[cellcount].x == cell[i].x && cell[cellcount].y == cell[i].y)
				check = true;
		}
	}
	cell[cellcount].size = maxcell;
	cellcount++;
}

void selectsquare()
{
	if (cellcount == 10) {
		for (int i = 0; i < maxcell - 1; i++)
			cell[i] = cell[i + 1];
		cellcount = 9;
	}
	cell[cellcount].shapetype = 3;
	cell[cellcount].color = colors[rand() % 4];
	bool check = true;
	while (check)
	{
		check = false;
		cell[cellcount].x = rand() % tablecount;
		cell[cellcount].y = rand() % tablecount;
		for (int i = 0; i < cellcount; i++) {
			if (cell[cellcount].x == cell[i].x && cell[cellcount].y == cell[i].y)
				check = true;
		}
	}
	cell[cellcount].size = maxcell;
	cellcount++;
}

void drawshape(HDC hdc, Cell cell)
{
	if (cell.shapetype == 0)
		return;
	int rx = cellsize / 2 + (cellsize * cell.x);
	int ry = cellsize / 2 + (cellsize * cell.y);
	switch (cell.shapetype)
	{
	case 1: {
		if (circlecheck) {
			HBRUSH brush = CreateSolidBrush(circlecolor);
			SelectObject(hdc, brush);
			circle(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
		else {
			HBRUSH brush = CreateSolidBrush(cell.color);
			SelectObject(hdc, brush);
			circle(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
	}
		break;
	case 2: {
		if (trianglecheck) {
			HBRUSH brush = CreateSolidBrush(trianglecolor);
			SelectObject(hdc, brush);
			triangle(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
		else {
			HBRUSH brush = CreateSolidBrush(cell.color);
			SelectObject(hdc, brush);
			triangle(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
	}
		break;
	case 3: {
		if (squarecheck) {
			HBRUSH brush = CreateSolidBrush(squarecolor);
			SelectObject(hdc, brush);
			square(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
		else {
			HBRUSH brush = CreateSolidBrush(cell.color);
			SelectObject(hdc, brush);
			square(hdc, rx, ry, cell.size);
			DeleteObject(brush);
		}
	}
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
		for (int i = 0; i < 10; ++i)
			drawshape(hdc, cell[i]);
		drawshape(hdc, player);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:

		if (wParam == 'Q')
			PostQuitMessage(0);
		else if (wParam == VK_UP) MovePlayer(0, -1);
		else if (wParam == VK_LEFT) MovePlayer(-1, 0);
		else if (wParam == VK_DOWN) MovePlayer(0, 1);
		else if (wParam == VK_RIGHT) MovePlayer(1, 0);
		else if (wParam == 'P') settinggame();
		else if (wParam == 'E') selectcircle();
		else if (wParam == 'T') selecttriangle();
		else if (wParam == 'R') selectsquare();
		else if (wParam == 'C') changecolor();
		else if (wParam == 'D') deletecell();
		else if (wParam == '0') selectplayer(9);
		else if (wParam == '1') selectplayer(0);
		else if (wParam == '2') selectplayer(1);
		else if (wParam == '3') selectplayer(2);
		else if (wParam == '4') selectplayer(3);
		else if (wParam == '5') selectplayer(4);
		else if (wParam == '6') selectplayer(5);
		else if (wParam == '7') selectplayer(6);
		else if (wParam == '8') selectplayer(7);
		else if (wParam == '9') selectplayer(8);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		if (wParam == '+') sizeup();
		else if (wParam == '-') sizedown();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}
