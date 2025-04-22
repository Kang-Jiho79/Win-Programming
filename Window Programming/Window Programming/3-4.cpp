//실습 3-4번
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
	int playercheck; // 0 : 플레이어아님 1 : 세로 2 : 가로 3 : 사각형
	int colortype;	// 0 : 흰 1 : 빨강 2 : 초록 3 ; 파랑
	int x, y;
	COLORREF color;
};

int rotationcheck = false;

bool startcheck = false;
Cell player[4];
Cell table[tablecount][tablecount];

COLORREF colors[4] = { RGB(255,255,255),RGB(255,0,0),RGB(0,255,0),RGB(0,0,255) };

// 게임 기초 세팅
void settinggame()
{
	srand(unsigned(time(NULL)));
	for (int i = 0; i < tablecount; i++)
		for (int j = 0; j < tablecount; j++) {
			table[i][j].playercheck = 0;
			table[i][j].colortype = 0;
			table[i][j].x = j;
			table[i][j].y = i;
			table[i][j].color = colors[0];
		}
	for (int i = 0; i < 4; i++) {
		player[i].playercheck = 0;
		player[i].colortype = 0;
		player[i].x = 14;
		player[i].y = 3;
		player[i].color = colors[0];
	}
}

//플레이어 생성
void playersetting()
{
	rotationcheck = false;
	int selectplayer = rand() % 3 + 1;
	for (int i = 0; i < 4; i++) {
		player[i].playercheck = selectplayer;
		player[i].colortype = selectplayer;
		player[i].color = colors[selectplayer];
		if (selectplayer == 1) {
			player[i].x = 14;
			player[i].y = 3 - i;
		}
		else if (selectplayer == 2) {
			player[i].x = 14+i;
			player[i].y = 0;
		}
		else {
			player[0].x = 14;
			player[0].y = 3;
			player[1].x = 15;
			player[1].y = 3;
			player[2].x = 14;
			player[2].y = 2;
			player[3].x = 15;
			player[3].y = 2;
		}
	}
}

// 게임 시작
void startgame() {
	startcheck = true;
	playersetting();
}

// 판 그리기
void drawtable(HDC hdc)
{
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}

// 플레이어 멈추게하기
void playermovestop()
{
	switch (player[0].playercheck)
	{
	case 1: {
		if (rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[player[0].y][player[0].x + i].colortype = player[0].playercheck;
				table[player[0].y][player[0].x + i].color = colors[player[0].playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[player[0].y - i][player[0].x].colortype = player[0].playercheck;
				table[player[0].y - i][player[0].x].color = colors[player[0].playercheck];
			}
		}
	}
		  break;
	case 2: {
		if (rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[player[0].y - i][player[0].x].colortype = player[0].playercheck;
				table[player[0].y - i][player[0].x].color = colors[player[0].playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[player[0].y][player[0].x + i].colortype = player[0].playercheck;
				table[player[0].y][player[0].x + i].color = colors[player[0].playercheck];
			}
		}
	}
		  break;
	case 3: {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				table[player[0].y - i][player[0].x + j].colortype = player[0].playercheck;
				table[player[0].y - i][player[0].x + j].color = colors[player[0].playercheck];
			}
	}
		  break;
	default:
		break;
	}
	playersetting();
}


// 플레이어 움직임
void MovePlayer(int dx, int dy) {
	int nx = player[0].x + dx;
	int ny = player[0].y + dy;
	if (nx < 0) return;
	else if (nx >= tablecount) return;
	else if (ny >= tablecount) {
		playermovestop();
		return;
	}
	switch (player[0].playercheck)
	{
	case 1: {
		if (rotationcheck) {
			if (player[3].x + dx >= tablecount)
				return;
			for (int i = 0; i < 4; i++) {
				if (table[ny][nx + i].colortype != 0) {
					if (dx == 0) {
						playermovestop();
						return;
					}
					else
						return;
				}
			}
			for (int i = 0; i < 4; i++) {
				player[i].x += dx;
				player[i].y += dy;
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				if (table[ny - i][nx].colortype != 0) {
					if (dx == 0) {
						playermovestop();
						return;
					}
					else
						return;
				}
			}
			for (int i = 0; i < 4; i++) {
				player[i].x += dx;
				player[i].y += dy;
			}
		}
	}
		  break;
	case 2: {
		if (rotationcheck) {
			for (int i = 0; i < 4; i++) {
				if (table[ny - i][nx].colortype != 0) {
					if (dx == 0) {
						playermovestop();
						return;
					}
					else
						return;
				}
			}
			for (int i = 0; i < 4; i++) {
				player[i].x += dx;
				player[i].y += dy;
			}
		}
		else {
			if (player[3].x + dx >= tablecount)
				return;
			for (int i = 0; i < 4; i++) {
				if (table[ny][nx + i].colortype != 0) {
					if (dx == 0) {
						playermovestop();
						return;
					}
					else
						return;
				}
			}
			for (int i = 0; i < 4; i++) {
				player[i].x += dx;
				player[i].y += dy;
			}
		}
	}
		  break;
	case 3: {
		if (player[1].x + dx >= tablecount)
			return;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				if (table[ny - i][nx + j].colortype != 0) {
					if (dx == 0) {
						playermovestop();
						return;
					}
					else
						return;
				}
			}
		for (int i = 0; i < 4; i++) {
			player[i].x += dx;
			player[i].y += dy;
		}
	}
		  break;
	default:
		break;
	}
}

// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

// 셀 설정
void drawshape(HDC hdc, Cell cell)
{
	int rx = cellsize / 2 + (cellsize * cell.x);
	int ry = cellsize / 2 + (cellsize * cell.y);
	HBRUSH brush = CreateSolidBrush(cell.color);
	SelectObject(hdc, brush);
	square(hdc, rx, ry, cellsize);
	DeleteObject(brush);
}

void tablecheck(int x, int y)
{
	if (x > tablecount - 4) return;
	else if (y > tablecount - 4)return;
	int check = table[y][x].colortype;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (check != table[y+i][x+j].colortype)
				return;
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			table[y+i][x+j].colortype = 0;
			table[y+i][x+j].color = colors[0];
		}
	}
}

void playerrotation()
{
	switch (player[0].playercheck)
	{
	case 1: {
		if (rotationcheck) {
			for (int i = 0; i < 4; i++) {
				player[i].x = player[0].x + i;
				player[i].y = player[0].y;
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				player[i].x = player[0].x;
				player[i].y = player[0].y - i;
			}
		}
	}
		  break;
	case 2: {
		if (rotationcheck) {
			for (int i = 0; i < 4; i++) {
				player[i].x = player[0].x;
				player[i].y = player[0].y - i;
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				player[i].x = player[0].x + i;
				player[i].y = player[0].y;
			}
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
		drawtable(mDC);
		for (int i = 0; i < tablecount; ++i)
			for (int j = 0; j < tablecount; j++) {
				drawshape(mDC, table[i][j]);
			}
		for (int i = 0; i < 4; i++)
			drawshape(mDC, player[i]);
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:

		if (wParam == 'Q')
			PostQuitMessage(0);
		if (startcheck) {
			if (wParam == VK_UP) MovePlayer(0, -(player[0].y - 3));
			else if (wParam == VK_LEFT) MovePlayer(-1, 0);
			else if (wParam == VK_DOWN) MovePlayer(0, 1);
			else if (wParam == VK_RIGHT) MovePlayer(1, 0);
			else if (wParam == VK_RETURN) {
				rotationcheck = ~rotationcheck;
				playerrotation();
			}
		}
		else
			if (wParam == 'S') {
				startgame();
			}
		for (int i = 0; i < tablecount; ++i)
			for (int j = 0; j < tablecount; j++)
				tablecheck(j, i);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER: {
		if (startcheck)
			if (player[0].playercheck != 0)
				MovePlayer(0, 1);
		for (int i = 0; i < tablecount; ++i)
			for (int j = 0; j < tablecount; j++)
				tablecheck(j, i);
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
