#define _CRT_SECURE_NO_WARNINGS
#pragma comment (lib, "msimg32.lib")
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "resource1.h"

#define chmax 30

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

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 800, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

	ShowWindow(hWnd, nCMdShow);	//	윈도우 띄우기
	UpdateWindow(hWnd);		// 윈도우 업데이트

	while (GetMessage(&Message, 0, 0, 0)) {		//	메시지 루프 (메시지 받아오는 역할) -  종료메시지가 들어올때까지 반복
		TranslateMessage(&Message);		//	메시지 번역
		DispatchMessage(&Message);	//	메시지 처리
	}
	return Message.wParam;
}

struct Charecter {
	double	x, y, width, length;
	double	dx, dy;
	bool	jump;
	bool	liedown;
	bool	type;		// true면 player
};

Charecter	player;
Charecter	obstacle;
HBITMAP		Sky, Ground;
HBITMAP		ch1[6];
HBITMAP		ch2[7];
HBITMAP		bomb;
POINT		skypoint;
POINT		groundpoint;
int			ch1width, ch1length;
int			ch2width, ch2length;
int			bombwidth, bomblength;
int			jumpcount;
bool		bombcheck = false;

void setting(RECT rt)
{
	skypoint = { 0,0 };
	groundpoint = { 0,rt.bottom / 2 };
	player.x = rt.right - 100;
	player.y = rt.bottom - 100;
	player.width = 50;
	player.length = 50;
	player.type = true;
	player.liedown = false;
	player.jump = false;
	obstacle.x = 0;
	obstacle.y = rt.bottom - 100;
	obstacle.width = 50;
	obstacle.length = 50;
	obstacle.type = false;
	obstacle.liedown = false;
	obstacle.jump = false;
	obstacle.dx = 1;
	obstacle.dy = 0;
}

void Animation(HDC hDC, RECT rt, Charecter c)
{
	HDC hMemDC;
	HBITMAP hBit, oldBit;
	static int count1;
	static int count2;
	int i;
	BITMAP bmp;
	count1++;
	count1 = count1 % 6;
	count2++;
	count2 = count2 % 7;
	hMemDC = CreateCompatibleDC(hDC);
	if (c.type) {
		(HBITMAP)SelectObject(hMemDC, ch1[count1]);
		TransparentBlt(hDC, c.x - c.width / 2, c.y, c.width, (c.liedown ? c.length /2 : c.length), hMemDC, 0, 0, ch1width, ch1length, RGB(0, 0, 64));
	}
	else {
		(HBITMAP)SelectObject(hMemDC, ch2[count2]);
		TransparentBlt(hDC, c.x - c.width / 2, c.y, c.width, (c.liedown ? c.length / 2 : c.length), hMemDC, 0, 0, ch2width, ch2length, RGB(0, 0, 64));
	}
	DeleteDC(hMemDC);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	BITMAP bmp;
	HDC hDC, mDC, hMemDC;
	HBITMAP HBitmap;
	RECT rt;
	GetClientRect(hWnd, &rt);
	switch (iMessage) {
	case WM_CREATE:
		srand(time(NULL));
		SetTimer(hWnd, 1, 10, NULL);
		setting(rt);
		Sky = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_SKY));
		Ground = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_GROUND));
		ch1[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_1));
		ch1[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_2));
		ch1[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_3));
		ch1[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_4));
		ch1[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_5));
		ch1[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch1_6));
		GetObject(ch1[0], sizeof(BITMAP), &bmp);
		ch1width = bmp.bmWidth;
		ch1length = bmp.bmHeight;
		ch2[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_1));
		ch2[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_2));
		ch2[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_3));
		ch2[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_4));
		ch2[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_5));
		ch2[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_6));
		ch2[6] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_Ch2_7));
		GetObject(ch1[0], sizeof(BITMAP), &bmp);
		ch2width = bmp.bmWidth;
		ch2length = bmp.bmHeight;
		bomb = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BOMB));
		GetObject(bomb, sizeof(BITMAP), &bmp);
		bombwidth = bmp.bmWidth;
		bomblength = bmp.bmHeight;
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		HBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)HBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		GetObject(Sky, sizeof(BITMAP), &bmp);
		HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP oldBit = (HBITMAP)SelectObject(hMemDC, Sky);
		StretchBlt(mDC, skypoint.x, skypoint.y, rt.right, rt.bottom / 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		StretchBlt(mDC, skypoint.x - rt.right, skypoint.y, rt.right, rt.bottom / 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		SelectObject(hMemDC, oldBit);
		GetObject(Ground, sizeof(BITMAP), &bmp);
		oldBit = (HBITMAP)SelectObject(hMemDC, Ground);
		StretchBlt(mDC, groundpoint.x, groundpoint.y, rt.right, rt.bottom / 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		StretchBlt(mDC, groundpoint.x - rt.right, groundpoint.y, rt.right, rt.bottom / 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		SelectObject(hMemDC, oldBit);
		Animation(mDC, rt, obstacle);
		if (bombcheck) {
			(HBITMAP)SelectObject(hMemDC, bomb);
			TransparentBlt(mDC, obstacle.x - obstacle.width / 2, obstacle.y - obstacle.length / 2, obstacle.width, obstacle.length, hMemDC, 0, 0, bombwidth, bomblength, RGB(0, 0, 64));
		}
		Animation(mDC, rt, player);
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteDC(hMemDC);
		DeleteObject(HBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CHAR:
	{
		if (wParam == 'j'){
			if (!player.jump) {
				if (player.liedown) {
					player.y -= player.length / 2;
					player.liedown = false;
				}
				player.jump = true;
				player.dy = -1;
				jumpcount = 200;
			}
		}
		else if (wParam == 'J') {
			if (player.liedown) player.y -= player.length / 2;
			else player.y += player.length / 2;
			player.liedown = !player.liedown;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT) {
			if (player.x > player.width/2 && player.x < rt.right - player.width/2)
				player.x--;
		}
		else if (wParam == VK_RIGHT) {
			if (player.x > player.width / 2 && player.x < rt.right - player.width / 2)
				player.x++;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_TIMER:
	{
		if (player.jump) {
			jumpcount--;
			player.y += player.dy;
			if (jumpcount == 0) {
				player.jump = false;
			}
			else if (jumpcount == 100) {
				player.dy = 1;
			}
		}
		obstacle.x++;
		skypoint.x += 10;
		groundpoint.x += 3;
		if (skypoint.x > rt.right)
			skypoint.x -= rt.right;
		if (groundpoint.x > rt.right)
			groundpoint.x -= rt.right;
		if (obstacle.x > rt.right + obstacle.width / 2) {
			int ran = rand() % 3;
			obstacle.x = -obstacle.width / 2;
			if (ran == 0) obstacle.y = rt.bottom - 100;
			else if (ran == 1) obstacle.y = rt.bottom - 125;
			else if (ran == 2) obstacle.y = rt.bottom - 150;
		}
		if (fabs(obstacle.x - player.x) < obstacle.width/2 + player.width/2 && fabs(obstacle.y - player.y) < obstacle.length/2 + player.length/2) {
			bombcheck = true;
		}
		else {
			bombcheck = false;
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