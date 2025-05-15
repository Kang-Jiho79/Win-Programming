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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// �����Լ� 
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;	//������ Ŭ����
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);	//	����ü�� ũ��
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//	������ ��� ��Ÿ��
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//	������ �޼����� ó���ϴ� ������ ���ν��� �Լ� �̸� (Long Pointer Funtion WndProc)
	WndClass.cbClsExtra = 0;	//	��뿩�� �޸�(class)
	WndClass.cbWndExtra = 0;	//	��뿩�� �޸�(Window)
	WndClass.hInstance = hInstance;		//���� ���α׷� ID
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// ���������� ���� ����������
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// �����쿡�� ���� Ŀ������
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// �������� ������ �����Ҽ��ִ�.
	WndClass.lpszMenuName = NULL;	//�޴� �̸�
	WndClass.lpszClassName = IpszClass;		//	Ŭ������ �̸�
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//���� ������ (���� hIcon�� ������ ���)
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1100, NULL, (HMENU)NULL, hInstance, NULL);	//������ ����� �Լ�

	ShowWindow(hWnd, nCMdShow);	//	������ ����
	UpdateWindow(hWnd);		// ������ ������Ʈ

	while (GetMessage(&Message, 0, 0, 0)) {		//	�޽��� ���� (�޽��� �޾ƿ��� ����) -  ����޽����� ���ö����� �ݺ�
		TranslateMessage(&Message);		//	�޽��� ����
		DispatchMessage(&Message);	//	�޽��� ó��
	}
	return Message.wParam;
}

struct Magnifier {
	double x, y, width, length;
};

Magnifier		magn;					// ������
Magnifier		copy[5];				// ������ ī��
int				copycount = 0;			// �ٿ��ֱ�� Ƚ��
bool			existmagn = false;		// �����Ⱑ �ִ���
double			magnification = 0;		// ������ ����
bool			copycheck = false;		// �����Ͽ�����
bool			fullcopy = false;		// ��üȭ�� �ٿ��ֱ�����
bool			revh = false;			// �¿����
bool			revv = false;			// ���Ϲ���
bool			bounce = false;			// �����Ⱑ ƨ��� �̵��� �ϴ���
bool			biggersmaller = false;	// �����Ⱑ Ŀ���� �۾����� �ϴ���
bool			reverse = false;		// �����Ǵ���
POINT			ptLT, ptRB;				// ���콺 Ŭ������ �̵�����
bool			makeact = false;		// ������ ����� ������
bool			moveact = false;		// ������ �̵� ������
bool			dragact = false;		// ������ ũ�� ����������
int				dx = 1;					// ƨ��� �̵�
int				dy = 1;					// Ŀ���� �۾�����

HBITMAP hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));

// �簢�� �׸���
void square(HDC hdc, int x1, int y1, int x2, int y2)
{
	Rectangle(hdc, x1, y1, x2, y2);
}

// �� ����
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

// ���纻 �׸���
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

// ������ �׸���
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