#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "resource1.h"

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

struct Magnifier {
	double x, y, width, length;
};

Magnifier		magn;					// 돋보기
Magnifier		copy[5];				// 돋보기 카피
int				copycount = 0;			// 붙여넣기된 횟수
bool			existmagn = false;		// 돋보기가 있는지
double			magnification = 0;		// 돋보기 배율
bool			copycheck = false;		// 복사하였는지
bool			fullcopy = false;		// 전체화면 붙여넣기인지
bool			revh = false;			// 좌우반전
bool			revv = false;			// 상하반전
bool			bounce = false;			// 돋보기가 튕기기 이동을 하는지
bool			biggersmaller = false;	// 돋보기가 커졌가 작아졌다 하는지
bool			reverse = false;		// 반전되는지
POINT			ptLT, ptRB;				// 마우스 클릭시점 이동시점
bool			makeact = false;		// 돋보기 만드는 중인지
bool			moveact = false;		// 돋보기 이동 중인지
bool			dragact = false;		// 돋보기 크기 변경중인지
int				dx = 1;					// 튕기기 이동
int				dy = 1;					// 커졌다 작아졌다

HBITMAP hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));

// 사각형 그리기
void square(HDC hdc, int x1, int y1, int x2, int y2)
{
	Rectangle(hdc, x1, y1, x2, y2);
}

// 셀 설정
void drawsquare(HDC hdc, Magnifier m)
{
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	HPEN pen = CreatePen(BS_SOLID, 10, RGB(255, 0, 0));
	HPEN oldpen = (HPEN)SelectObject(hdc, pen);
	square(hdc, m.x, m.y, m.x + m.width, m.y + m.length);
	SelectObject(hdc, oldpen);
	SelectObject(hdc, oldbrush);
	DeleteObject(pen);
	DeleteObject(brush);
}

// 복사본 그리기
void drawcopy(HDC hdc, HDC mdc, RECT rt, BITMAP bmp, Magnifier m)
{
	StretchBlt(hdc, m.x, m.y, m.width, m.length, 
		mdc, 
		(revh ? ((magn.width / rt.right) * bmp.bmWidth - magnification)+ ((magn.x / rt.right) * bmp.bmWidth + magnification) : ((magn.x / rt.right) * bmp.bmWidth + magnification)),
		(revv ? ((magn.length / rt.bottom) * bmp.bmHeight - magnification) + ((magn.y / rt.bottom) * bmp.bmHeight + magnification) : ((magn.y / rt.bottom) * bmp.bmHeight + magnification)),
		(revh ? -1 *((magn.width / rt.right) * bmp.bmWidth - magnification) : ((magn.width / rt.right) * bmp.bmWidth - magnification)), 
		(revv ? -1 *((magn.length / rt.bottom) * bmp.bmHeight - magnification) : ((magn.length / rt.bottom) * bmp.bmHeight - magnification)), (reverse ? NOTSRCCOPY : SRCCOPY));
	drawsquare(hdc, m);
}

// 돋보기 그리기
void drawmagn(HDC hdc, HDC mdc, RECT rt, BITMAP bmp, Magnifier m)
{
	StretchBlt(hdc, m.x, m.y, m.width, m.length,
		mdc, (magn.x / rt.right) * bmp.bmWidth + magnification, (magn.y / rt.bottom) * bmp.bmHeight + magnification,
		(magn.width / rt.right) * bmp.bmWidth - magnification,
		(magn.length / rt.bottom) * bmp.bmHeight - magnification, (reverse ? NOTSRCCOPY : SRCCOPY));
	drawsquare(hdc, m);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	BITMAP bmp;
	HDC hDC, srcDC, mDC;
	HBITMAP HBitmap;
	RECT rt;
	GetClientRect(hWnd, &rt);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 10, NULL);
		hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		srcDC = CreateCompatibleDC(NULL);
		HBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)HBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		SelectObject(srcDC, (HBITMAP)hBitmap);
		StretchBlt(mDC, 0, 0, rt.right, rt.bottom, srcDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		if (existmagn) {
			if (copycheck) {
				if (fullcopy)
					StretchBlt(mDC, 0, 0, rt.right, rt.bottom, srcDC, (magn.x / rt.right) * bmp.bmWidth + magnification, (magn.y / rt.bottom) * bmp.bmHeight + magnification, (magn.width / rt.right) * bmp.bmWidth + magnification, (magn.length / rt.bottom) * bmp.bmHeight + magnification, SRCCOPY);
				else {
					for (int i = 0; i < copycount; ++i) {
						drawcopy(mDC, srcDC, rt, bmp, copy[i]);
					}
				}
			}
			if(!fullcopy)
				drawmagn(mDC, srcDC, rt, bmp, magn);
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(HBitmap);
		DeleteDC(srcDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN: 
	{
		if (wParam == VK_UP) {
			for (int i = 0; i < 5; ++i)
				if (magn.y > 0)
					magn.y -= 1;
			}
		else if (wParam == VK_RIGHT) {
			for (int i = 0; i < 5; ++i)
				if (magn.x + magn.width < rt.right)
					magn.x += 1;
		}
		else if (wParam == VK_DOWN) {
			for (int i = 0; i < 5; ++i)
				if (magn.y + magn.length < rt.bottom)
					magn.y += 1;
		}
		else if (wParam == VK_LEFT) {
			for (int i = 0; i < 5; ++i)
				if (magn.x > 0)
					magn.x -= 1;
		}
	}
	break;
	case WM_CHAR:
		if (wParam == 'q') {
			PostQuitMessage(0);
		}
		else if (wParam == '1') {
			hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		}
		else if (wParam == '2') {
			hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		}
		else if (wParam == 'e') {
			if (magnification == 20) magnification = 0;
			else magnification += 1;
		}
		else if (wParam == 's') {
			if (magnification == -20) magnification = 0;
			else magnification -= 1;
		}
		else if (wParam == 'b') {
			magnification = 0;
		}
		else if (wParam == 'c') {
			copycheck = true;
		}
		else if (wParam == 'p') {
			if (copycheck) {
				if (copycount < 5) {
					srand(time(NULL));
					copy[copycount].x = rand() % (int)(rt.right - magn.width);
					copy[copycount].y = rand() % (int)(rt.bottom - magn.length);
					copy[copycount].width = magn.width / 2;
					copy[copycount].length = magn.length / 2;
					copycount++;
				}
			}
		}
		else if (wParam == 'f') {
			if (copycheck) {
				fullcopy = !fullcopy;
			}
		}
		else if (wParam == 'h') {
			revh = !revh;
		}
		else if (wParam == 'v') {
			revv = !revv;
		}
		else if (wParam == 'm') {
			bounce = !bounce;
		}
		else if (wParam == 'n') {
			biggersmaller = !biggersmaller;
		}
		else if (wParam == 'i') {
			reverse = !reverse;
		}
		else if (wParam == 'r') {
			magn.x = 0;
			magn.y = 0;
			magn.length = 0;
			magn.width = 0;
			for (int i = 0; i < copycount; ++i) {
				copy[i].x = 0;
				copy[i].y = 0;
				copy[i].width = 0;
				copy[i].length = 0;
			}
			copycount = 0;			
			existmagn = false;		
			magnification = 0;		
			copycheck = false;		
			fullcopy = false;		
			revh = false;			
			revv = false;			
			bounce = false;			
			biggersmaller = false;	
			reverse = false;				
			makeact = false;		
			moveact = false;		
			dragact = false;		
			dx = 1;					
			dy = 1;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	
	case WM_LBUTTONDOWN:
	{
		ptLT.x = LOWORD(lParam);
		ptLT.y = HIWORD(lParam);
		if (existmagn) {
			if (ptLT.x >= magn.x + 10 && ptLT.x <= magn.x + magn.width - 10 && ptLT.y >= magn.y + 10 && ptLT.y <= magn.y + magn.length - 10) {
				moveact = true;
			}
			else if (ptLT.x >= magn.x - 10 && ptLT.x <= magn.x + magn.width + 10 && ptLT.y >= magn.y - 10 && ptLT.y <= magn.y + magn.length + 10) {
				dragact = true;
			}
		}
		else {
			magn.x = ptLT.x;
			magn.y = ptLT.y;
			makeact = true;
			existmagn = true;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_MOUSEMOVE:
	{
		ptRB.x = LOWORD(lParam);
		ptRB.y = HIWORD(lParam);
		if (moveact) {
			magn.x += ptRB.x - ptLT.x;
			magn.y += ptRB.y - ptLT.y;
			ptLT.x = ptRB.x;
			ptLT.y = ptRB.y;
		}
		else if (dragact) {
			if (ptRB.x < magn.x) {
				magn.x = ptRB.x;
				magn.width += ptLT.x - ptRB.x;
			}
			else if(ptLT.x > magn.x + magn.width/2) magn.width += ptRB.x - ptLT.x;
			else magn.width += ptLT.x - ptRB.x;
			if (ptLT.y < magn.y) {
				magn.y = ptRB.y;
				magn.length += ptLT.y - ptRB.y;
			}
			else if (ptLT.y > magn.y + magn.length / 2) magn.length += ptRB.y - ptLT.y;
			else magn.length += ptLT.y - ptRB.y;
			ptLT.x = ptRB.x;
			ptLT.y = ptRB.y;
		}
		else if (makeact) {
			if (ptRB.x < ptLT.x) {
				magn.x = ptRB.x;
				magn.width = ptLT.x - ptRB.x;
			}
			else {
				magn.width = ptRB.x - ptLT.x;
			}
			if (ptRB.y < ptLT.y) {
				magn.y = ptRB.y;
				magn.width = ptLT.y - ptRB.y;
			}
			else {
				magn.length = ptRB.y - ptLT.y;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_LBUTTONUP:
	{
		ptRB.x = LOWORD(lParam);
		ptRB.y = HIWORD(lParam);
		if (moveact) {
			moveact = false;
			magn.x += ptRB.x - ptLT.x;
			magn.y += ptRB.y - ptLT.y;
		}
		else if (dragact) {
			dragact = false;
			if (ptRB.x < magn.x) {
				magn.x = ptRB.x;
				magn.width += ptLT.x - ptRB.x;
			}
			else if (ptLT.x > magn.x + magn.width / 2) magn.width += ptRB.x - ptLT.x;
			else magn.width += ptLT.x - ptRB.x;
			if (ptLT.y < magn.y) {
				magn.y = ptRB.y;
				magn.length += ptLT.y - ptRB.y;
			}
			else if (ptLT.y > magn.y + magn.length / 2) magn.length += ptRB.y - ptLT.y;
			else magn.length += ptLT.y - ptRB.y;
		}
		else if (makeact) {
			makeact = false;
			if (ptRB.x < ptLT.x) {
				magn.x = ptRB.x;
				magn.width = ptLT.x - ptRB.x;
			}
			else {
				magn.width = ptRB.x - ptLT.x;
			}
			if (ptRB.y < ptLT.y) {
				magn.y = ptRB.y;
				magn.width = ptLT.y - ptRB.y;
			}
			else {
				magn.length = ptRB.y - ptLT.y;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_TIMER:
	{
		if (bounce) {
			for (int i = 0; i < 5; ++i) {
				if (magn.x + magn.width >= rt.right) dx = -1;
				if (magn.x <= 0) dx = 1;
				magn.x += dx;
			}
		}
		if (biggersmaller) {
			for (int i = 0; i < 5; ++i) {
				if (magn.x + magn.width >= rt.right || magn.y + magn.length >= rt.bottom || magn.x <= 0 || magn.y <= 0) dy = -1;
				if (magn.width <= 0 || magn.length <= 0) dy = 1;
				magn.x -= dy;
				magn.y -= dy;
				magn.width += 2*dy;
				magn.length += 2*dy;
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