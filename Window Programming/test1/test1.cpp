//실습 13번
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

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

	ShowWindow(hWnd, nCMdShow);	//	윈도우 띄우기
	UpdateWindow(hWnd);		// 윈도우 업데이트

	while (GetMessage(&Message, 0, 0, 0)) {		//	메시지 루프 (메시지 받아오는 역할) -  종료메시지가 들어올때까지 반복
		TranslateMessage(&Message);		//	메시지 번역
		DispatchMessage(&Message);	//	메시지 처리
	}
	return Message.wParam;
}

struct Player {
	int x, y, speed, size;
	int movecheck;
	bool width, length;
};



Player player;

// 게임 기초 세팅
void settinggame()
{
	player.x = 20;
	player.y = 20;
	player.size = 20;
	player.speed = 5;
	player.movecheck = 0;
	player.width = true;
	player.length = true;
}


void MovePlayer(int dx, int dy) {
	player.x += dx;
	player.y += dy;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	static RECT rectView;
	switch (iMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rectView);
		SetTimer(hWnd, 1, 1000 * player.speed, NULL);
		settinggame();
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		circle(hDC, player.x, player.y,player.size);
		EndPaint(hWnd, &ps);
		break;
	case WM_CHAR:
		if (wParam == 'h')
			player.movecheck = 1;
		else if (wParam == 'v')
			player.movecheck = 2;
		else if (wParam == 'p')
			player.movecheck = 0;
		else if (wParam == '-') {
			if (player.speed > 1) {
				player.speed--;
			}
		}
		else if (wParam == '+') {
			if (player.speed < 10) {
				player.speed++;
			}
		}
		else if (wParam == 'q')
			PostQuitMessage(0);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		player.x = LOWORD(lParam);
		player.y = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		if (player.movecheck == 1) {
			if (player.x + player.size / 2 > 800) {
				player.width = false;
				if (player.y + player.size > 600)
					player.y = 20;
				else 
					player.y += cellsize;
			}
			else if (player.x - player.size / 2 < 0) {
				player.width = true;
				if (player.y + player.size > 600)
					player.y = 20;
				else
					player.y += cellsize;
			}
			if (player.width) { MovePlayer(player.speed, 0); }
			else { MovePlayer(-player.speed, 0); }
		}
		else if (player.movecheck == 2) {
			if (player.y + player.size / 2 > 600) {
				player.length = false;
				if (player.x + player.size > 800)
					player.x = 20;
				else
					player.x += cellsize;
			}
			else if (player.y - player.size / 2 < 0) {
				player.length = true;
				if (player.x + player.size > 800)
					player.x = 20;
				else
					player.x += cellsize;
			}
			if (player.length) { MovePlayer(0, player.speed); }
			else { MovePlayer(0, -player.speed); }
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}