//실습 12번
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 20
#define cellsize 20

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
};

struct Cell {
	bool check = false;
	int wordnumber = 0;
};

Player player;
Cell table[tablecount][tablecount];
TCHAR words[5][6] = { L"APPLE", L"WORLD", L"CLASS", L"TABLE", L"STUDY" };
int r;
	


// 게임 기초 세팅
void settinggame()
{
	srand((unsigned)time(0));
	r = rand() % 5;
	for (int i =0;i<tablecount; i++)
		for (int j = 0; j < tablecount; j++) {
			table[i][j].check = false;
			table[i][j].wordnumber = 0;
		}
	for (int i = 0; i < 5; i++) {
		if (i == r) {
			table[1][8 + i].wordnumber = 0;
			table[1][8 + i].check = false;
		}
		else {
			table[1][8 + i].wordnumber = words[r][i] - 'A';
			table[1][8 + i].check = true;
		}
	}
	for (int i = 3; i < tablecount-1; ++i) {
		for (int j = 1; j < tablecount-1; ++j) {
			int r = rand() % 10;
			if (r == 0) {
				table[i][j].wordnumber =rand() % 26;
				table[i][j].check = true;
			}
		}
	}
	player.x = 10;
	player.y = 10;
}




void drawtable(HDC hdc)
{
	for (int i = 0; i < tablecount; i++) {
		for (int j = 0; j < tablecount; j++) {
			if (table[i][j].check == true) {
				TCHAR ch = 'A' + table[i][j].wordnumber;
				TextOut(hdc, ((j * cellsize) + cellsize / 2)-5, ((i * cellsize) + cellsize / 2)-5, &ch , 1);
			}
		}
	}
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}

int wordnumadd(int a, int b)
{
	return (a + b > 25 ? a + b - 25 : a + b+1);
}

void MovePlayer(int dx, int dy) {
	int nx = player.x + dx;
	int ny = player.y + dy;
	if (nx < 0 || ny < 0 || nx >= tablecount || ny >= tablecount)
		return;
	if (table[ny][nx].check)
		if(nx + dx < 0 || ny + dy < 0 || nx + dx >= tablecount || ny + dy >= tablecount)
			return;
	player.x = nx;
	player.y = ny;

	if (table[ny][nx].check) {
		if (table[ny + dy][nx + dx].check) {
			table[ny + dy][nx + dx].wordnumber = wordnumadd(table[ny + dy][nx + dx].wordnumber, table[ny][nx].wordnumber);
		}
		else {
			table[ny + dy][nx + dx].wordnumber = table[ny][nx].wordnumber;
			table[ny + dy][nx + dx].check = true;
		}
		table[ny][nx].check = false;
		table[ny][nx].wordnumber = 0;
	}
}


// 원 그리기
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

void drawshape(HDC hdc, int x, int y)
{
	int rx = cellsize / 2 + (cellsize * x);
	int ry = cellsize / 2 + (cellsize * y);
	HBRUSH brush = CreateSolidBrush(RGB(0,255,255));
	SelectObject(hdc, brush);
	circle(hdc, rx, ry, cellsize);
	DeleteObject(brush);
}

bool endcheck()
{
	bool endchecking = true;
	for (int i = 0; i < 5; i++) {
		if (table[1][8 + i].wordnumber != words[r][i] - 'A' && !table[1][8+i].check)
			endchecking = false;
	}
	return endchecking;
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
		drawshape(hdc, player.x, player.y);
		if (endcheck())
			TextOut(hdc, 0, cellsize * tablecount+10, L"단어 완성!", _tcslen(L"단어 완성!"));
		else
			TextOut(hdc, 0, cellsize * tablecount+10, words[r], _tcslen(words[r]));
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (!endcheck()) {
			if (wParam == VK_UP) MovePlayer(0, -1);
			else if (wParam == VK_LEFT) MovePlayer(-1, 0);
			else if (wParam == VK_DOWN) MovePlayer(0, 1);
			else if (wParam == VK_RIGHT) MovePlayer(1, 0);
		}
		if (wParam == 'Q')
			PostQuitMessage(0);
		else if (wParam == 'S')
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
