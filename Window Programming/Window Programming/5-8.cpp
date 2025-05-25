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

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1100, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

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
	bool	type;		// true면 고양이 사진
};

Charecter	cat;
Charecter	mouse;
HBITMAP		hBitmap;
POINT		ptLT, ptRB;
HBITMAP		ch1[6];
HBITMAP		ch2[7];
int			ch1width, ch1length;
int			ch2width, ch2length;
bool		act = false;
bool		other = false;
int			othercount = 0;

void setting(RECT rt)
{
	cat.x = rt.right / 2;
	cat.y = rt.bottom / 2;
	cat.width = 50;
	cat.length = 100;
	cat.dx = 0;
	cat.dy = 0;
	cat.type = true;
	mouse.x = 0;
	mouse.y = 0;
	mouse.width = 50;
	mouse.length = 100;
	mouse.type = false;
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
		TransparentBlt(hDC, c.x, c.y, c.width, c.length, hMemDC, 0, 0, ch1width, ch1length, RGB(0, 0, 64));
	}
	else {
		(HBITMAP)SelectObject(hMemDC, ch2[count2]);
		TransparentBlt(hDC, c.x, c.y, c.width, c.length, hMemDC, 0, 0, ch2width, ch2length, RGB(0, 0, 64));
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
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	switch (iMessage) {
	case WM_CREATE:
		srand(time(NULL));
		SetTimer(hWnd, 1, 50, NULL);
		setting(rt);
		hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
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
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		HBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)HBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		HBITMAP hBit = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBit, sizeof(BITMAP), &bmp);
		HDC hMemDC = CreateCompatibleDC(mDC);
		HBITMAP oldBit = (HBITMAP)SelectObject(hMemDC, hBit);
		StretchBlt(mDC, 0, 0, rt.right, rt.bottom, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		SelectObject(hMemDC, oldBit);
		Animation(mDC, rt, cat);
		if (act) {
			Animation(mDC, rt, mouse);
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteDC(hMemDC);
		DeleteObject(hBit);
		DeleteObject(HBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		ptLT.x = LOWORD(lParam);
		ptLT.y = HIWORD(lParam);
		mouse.x = ptLT.x;
		mouse.y = ptLT.y;
		act = true;
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (act) {
			ptLT.x = LOWORD(lParam);
			ptLT.y = HIWORD(lParam);
			mouse.x = ptLT.x;
			mouse.y = ptLT.y;
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (act) {
			act = false;
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		ptRB.x = LOWORD(lParam);
		ptRB.y = HIWORD(lParam);
		if (ptRB.x >= cat.x && ptRB.x <= cat.x + cat.width && ptRB.y >= cat.y && ptRB.y <= cat.y + cat.length) {
			cat.type = false;
			other = true;
			othercount = 100;
		}
	}
	break;
	case WM_TIMER:
	{
		if (act) {
			cat.dx = (mouse.x - cat.x >= 0 ? 5 : -5);
			cat.dy = (mouse.y - cat.y >= 0 ? 5 : -5);
		}
		else {
			cat.dx = (rand() % 3 - 1) * 5;
			cat.dy = (rand() % 3 - 1) * 5;
		}
		if (other) {
			othercount--;
			if (othercount == 0) {
				other = false;
				cat.type = true;
			}
		}
		if (cat.x + cat.dx >= 0 && cat.x + cat.dx <= rt.right - cat.width) {
			cat.x += cat.dx;
		}
		if (cat.y + cat.dy >= 0 && cat.y + cat.dy <= rt.bottom - cat.length) {
			cat.y += cat.dy;
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