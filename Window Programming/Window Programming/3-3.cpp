#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define center_x 400
#define center_y 300
#define r 300
#define cellsize 30

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

struct Cell
{
	int x, y;
	int size;
};

struct Player
{
	int currentcount;
	int movecount;
	bool shortcut;
	int onemore;
	COLORREF color;
};

Cell cell[33];
Player player[2];
bool startcheck = false;
int turncheck = 0;
bool movingcheck = false;
bool yutcheck = false;

void settinggame()
{
	srand(unsigned(time(NULL)));
	cell[0].x = center_x - r / 2;
	cell[0].y = center_y + r / 2;
	for (int i = 0; i < 5; i++) {
		cell[i + 1].x = cell[i].x + cellsize * 2;
		cell[i + 1].y = cell[i].y;
	}
	for (int i = 5; i < 10; i++) {
		cell[i + 1].x = cell[i].x;
		cell[i + 1].y = cell[i].y - cellsize * 2;
	}
	for (int i = 10; i < 15; i++) {
		cell[i + 1].x = cell[i].x - cellsize * 2;
		cell[i + 1].y = cell[i].y;
	}
	for (int i = 15; i < 19; i++) {
		cell[i + 1].x = cell[i].x;
		cell[i + 1].y = cell[i].y + cellsize * 2;
	}
	cell[20].x = cell[5].x - r / 6;
	cell[20].y = cell[5].y - r / 6;
	for (int i = 20; i < 24; i++) {
		cell[i + 1].x = cell[i].x - r / 6;
		cell[i + 1].y = cell[i].y - r / 6;
	}
	cell[25].x = cell[10].x - r / 6;
	cell[25].y = cell[10].y + r / 6;
	for (int i = 25; i < 31; i++) {
		cell[i + 1].x = cell[i].x - r / 6;
		cell[i + 1].y = cell[i].y + r / 6;
	}
	cell[31].x = cell[24].x;
	cell[31].y = cell[24].y;
	cell[32].x = cell[23].x;
	cell[32].y = cell[23].y;
	for (int i = 0; i < 33; i++) {
		if (i == 0 || i == 5 || i == 10 || i == 15 || i == 22 || i == 27 || i == 30)
			cell[i].size = 40;
		else
			cell[i].size = 30;
	}
	for (int i = 0; i < 2; i++) {
		player[i].currentcount = 0;
		player[i].movecount = 0;
		player[i].onemore = 0;
		player[i].shortcut = false;
	}
	player[0].color = RGB(0, 0, 255);
	player[1].color = RGB(255, 0, 0);
}

void moveplayer(Player& p)
{
	if (p.movecount == 0) {
		if (p.currentcount == 5 || p.currentcount == 10 || p.currentcount == 15 || p.currentcount == 27)
			p.shortcut = true;
		else if (p.currentcount == 22) {
			p.currentcount = 27;
			p.shortcut = true;
		}
		return;
	}
	p.movecount--;
	if (p.shortcut) {
		p.shortcut = false;
		if (p.currentcount == 5 || p.currentcount == 10)
			p.currentcount += 15;
		else if (p.currentcount == 15)
			p.currentcount = 31;
		else
			p.currentcount = 28;
	 }
	else {
		if (p.currentcount == 24)
			p.currentcount = 15;
		else if (p.currentcount == 32)
			p.currentcount = 27;
		else if (p.currentcount == 19)
			p.currentcount = 30;
		else if (p.currentcount == 30)
			PostQuitMessage(0);
		else
			p.currentcount += 1;
	}
}

// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

void Yut(HDC hdc, int x, int y)
{
	Rectangle(hdc, x - 50 / 2, y - 100 / 2, x + 50 / 2, y + 100 / 2);
	int ran = rand() % 2;
	if (ran) {
		for (int i = 0; i < 3; i++) {
			int dy = y - 25 + 25 * i;
			MoveToEx(hdc, x-10, dy - 10, NULL);
			LineTo(hdc, x+10, dy+10);
			MoveToEx(hdc, x + 10, dy - 10, NULL);
			LineTo(hdc, x - 10, dy + 10);
		}
		player[turncheck].movecount++;
	}
}

void onemorecheck()
{
	if (player[turncheck].movecount == 0) {
		player[turncheck].movecount = 4;
		player[turncheck].onemore++;
	}
	else if (player[turncheck].movecount == 4) {
		player[turncheck].movecount = 5;
		player[turncheck].onemore++;
	}
}

void turnendcheck()
{
	if (turncheck == 0) {
		if (player[0].currentcount == player[1].currentcount)
			player[turncheck].onemore++;

		if (player[turncheck].onemore > 0)
			player[turncheck].onemore--;
		else
			turncheck = 1;
	}
	else if (turncheck == 1) {
		if (player[0].currentcount == player[1].currentcount)
			player[turncheck].onemore++;

		if (player[turncheck].onemore > 0)
			player[turncheck].onemore--;
		else
			turncheck = 0;
	}
	movingcheck = false;
}

void drawplayer(HDC hdc, Player p)
{
	HBRUSH brush = CreateSolidBrush(p.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	square(hdc, cell[p.currentcount].x, cell[p.currentcount].y, cellsize);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, mDC;
	HBITMAP hBitmap;
	RECT rt;
	switch (iMessage) {
	case WM_CREATE:
		settinggame();
		SetTimer(hWnd, 1, 1000, NULL);
		break;
	case WM_PAINT:
	{
		GetClientRect(hWnd, &rt);
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		if (startcheck) {
			if (turncheck == 0) {
				TextOut(mDC, 0, 0, L"플레이어차례", _tcslen(L"플레이어차례"));
			}
			else {
				TextOut(mDC, 0, 0, L"AI차례", _tcslen(L"AI차례"));
			}
		}
		else {
			TextOut(mDC, 0, 0, L"게임시작 전", _tcslen(L"게임시작 전"));
		}
		for (int i = 0; i < 33; i++) {
			square(mDC, cell[i].x, cell[i].y, cell[i].size);
		}
		for (int i = 0; i < 2; i++) {
			drawplayer(mDC, player[i]);
		}
		if (yutcheck) {
			for (int i = 0; i < 4; i++) {
				Yut(mDC, 100 * (i + 1), 300);
			}
			onemorecheck();
			yutcheck = false;
			movingcheck = true;
		}

		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (!movingcheck) {
			if (wParam == 'P')
				startcheck = true;
			if (startcheck) {
				if (wParam == 'Q')
					PostQuitMessage(0);
				else if (wParam == 's')
				{
					settinggame();
				}
				if (turncheck == 0) {
					if (wParam == '1') {
						yutcheck = true;
					}
				}
				else {
					if (wParam == '2') {
						yutcheck = true;
					}
				}
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER: {
		if (movingcheck) {
			moveplayer(player[0]);
			moveplayer(player[1]);
			if (player[turncheck].movecount == 0) {
				turnendcheck();
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
				 break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}