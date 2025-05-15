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
	double x, y, width, length;
	bool rect;
};

Charecter	ch[chmax];
HBITMAP		hBitmap;
bool		existrect = false;
bool		startgame = false;
POINT		ptLT, ptRB;
HBITMAP		ch1[6];
HBITMAP		ch2[7];
int			ch1width, ch1length;
int			ch2width, ch2length;
bool		act = false;
bool		nosquare = false;

void setting(RECT rt)
{
	srand(time(NULL));
	for (int i = 0; i < chmax; ++i) {
		ch[i].x = rand() % rt.right;
		ch[i].y = 0;
		ch[i].width = 20;
		ch[i].length = 20;
		ch[i].rect = false;
	}
}

void Animation(HDC hDC, RECT rt)
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
	hBit = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	GetObject(hBit, sizeof(BITMAP), &bmp);
	hMemDC = CreateCompatibleDC(hDC);
	oldBit = (HBITMAP)SelectObject(hMemDC, hBit);
	StretchBlt(hDC, 0, 0, rt.right, rt.bottom, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	if (startgame) {
		for (int i = 0; i < chmax; ++i) {
			if (ch[i].rect) {
				(HBITMAP)SelectObject(hMemDC, ch2[count2]);
				TransparentBlt(hDC, ch[i].x, ch[i].y, ch[i].width, ch[i].length, hMemDC, 0, 0, ch2width, ch2length, RGB(0, 0, 0));
			}
			else {
				(HBITMAP)SelectObject(hMemDC, ch1[count1]);
				TransparentBlt(hDC, ch[i].x, ch[i].y, ch[i].width, ch[i].length, hMemDC, 0, 0, ch1width, ch1length, RGB(0, 0, 0));
			}
		}
	}
	SelectObject(hMemDC, oldBit);
	DeleteDC(hMemDC);
	DeleteObject(hBit);
}


// 사각형 그리기
void square(HDC hdc, int x1, int y1, int x2, int y2)
{
	Rectangle(hdc, x1, y1, x2, y2);
}

// 셀 설정
void drawsquare(HDC hdc)
{
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	HPEN pen = CreatePen(BS_SOLID, 1, RGB(255, 0, 0));
	HPEN oldpen = (HPEN)SelectObject(hdc, pen);
	square(hdc, ptLT.x,ptLT.y,ptRB.x,ptRB.y);
	SelectObject(hdc, oldpen);
	SelectObject(hdc, oldbrush);
	DeleteObject(pen);
	DeleteObject(brush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	BITMAP bmp;
	HDC hDC, mDC;
	HBITMAP HBitmap;
	RECT rt;
	GetClientRect(hWnd, &rt);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 10, NULL);
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
		Animation(mDC, rt);
		if (act || existrect && !nosquare) {
			drawsquare(mDC);
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(HBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CHAR:
		if (wParam == 'q') {
			PostQuitMessage(0);
		}
		else if (wParam == 'p') {
			startgame = true;
		}
		else if (wParam == 'd') {
			nosquare = !nosquare;
		}
		else if (wParam == 'r') {
			existrect = false;
			for (int i = 0; i < chmax; ++i) {
				ch[i].rect = false;
			}
			ptLT = { 0,0 };
			ptRB = { 0,0 };
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
	{
		if (!existrect) {
			ptLT.x = LOWORD(lParam);
			ptLT.y = HIWORD(lParam);
			act = true;
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (act) {
			ptRB.x = LOWORD(lParam);
			ptRB.y = HIWORD(lParam);
			if (ptLT.x > ptRB.x) {
				int temp = ptLT.x;
				ptLT.x = ptRB.x;
				ptRB.x = temp;
			}
			if (ptLT.y > ptRB.y) {
				int temp = ptLT.y;
				ptLT.y = ptRB.y;
				ptRB.y = temp;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (act) {
			ptRB.x = LOWORD(lParam);
			ptRB.y = HIWORD(lParam);
			act = false;
			existrect = true;
			if (ptLT.x > ptRB.x) {
				int temp = ptLT.x;
				ptLT.x = ptRB.x;
				ptRB.x = temp;
			}
			if (ptLT.y > ptRB.y) {
				int temp = ptLT.y;
				ptLT.y = ptRB.y;
				ptRB.y = temp;
			}
			for (int i = 0; i < chmax; ++i) {
				if (ch[i].x > ptLT.x && ch[i].x + ch[i].width < ptRB.x)
					ch[i].rect = true;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_TIMER:
	{
		if (startgame) {
			for (int i = 0; i < chmax; ++i) {
				ch[i].y += 3;
				if (ch[i].rect) {
					if (ch[i].y + ch[i].length > ptRB.y) {
						ch[i].y = ptRB.y - ch[i].length;
					}
				}
				if (ch[i].y + ch[i].length > rt.bottom) {
					ch[i].y = 0;
				}
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