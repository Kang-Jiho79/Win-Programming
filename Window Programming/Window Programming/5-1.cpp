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

POINT pt[4][2] = { {{0,0},{255,255}}, {{0,0},{0,0}}, {{0,0},{0,0}}, {{0,0},{0,0}} };

int count = 1;
int choose = 0;
bool reverse = false;

// �簢�� �׸���
void square(HDC hdc, int x1, int y1, int x2, int y2)
{
	Rectangle(hdc, x1, y1, x2, y2);
}

// �� ����
void draw(HDC hdc, POINT pt[][2], int i)
{
	HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	HPEN pen = CreatePen(BS_SOLID, 10, RGB(255, 0, 0));
	HPEN oldpen = (HPEN)SelectObject(hdc, pen);
	square(hdc, pt[i][0].x, pt[i][0].y, pt[i][1].x + pt[i][0].x, pt[i][1].y);
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
	HBITMAP hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));;
	RECT rt;
	GetClientRect(hWnd, &rt);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	switch (iMessage) {
	case WM_CREATE:

		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		SelectObject(mDC, (HBITMAP)hBitmap);
		for (int i = 0; i < 4; ++i) {
			if (count == i) break;
			if (choose == i + 1) {
				if (reverse)
					StretchBlt(hDC, pt[i][0].x, pt[i][0].y, pt[i][1].x, pt[i][1].y, mDC, 0, 0, bmp.bmWidth, bmp.bmHeight, NOTSRCCOPY);
				else
					StretchBlt(hDC, pt[i][0].x, pt[i][0].y, pt[i][1].x, pt[i][1].y, mDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
				draw(hDC, pt, i);
			}
			else {
				StretchBlt(hDC, pt[i][0].x, pt[i][0].y, pt[i][1].x, pt[i][1].y, mDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			}

		}

		DeleteDC(mDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CHAR:
		if (wParam == 'a') {
			if (count == 1) {
				pt[0][0].x = 0;
				pt[0][0].y = 0;
				pt[0][1].x = rt.right;
				pt[0][1].y = rt.bottom;
			}
		}
		else if (wParam == '+') {
			if (count == 1) {
				if (pt[0][1].x + 10 > rt.right) {
					pt[0][1].x = rt.right;
				}
				else {
					pt[0][1].x += 10;
				}
				if (pt[0][1].y + 10 > rt.bottom) {
					pt[0][1].y = rt.bottom;
				}
				else {
					pt[0][1].y += 10;
				}
			}
		}
		else if (wParam == '-') {
			if (count == 1) {
				if (pt[0][1].x - 10 < rt.left) {
					pt[0][1].x = rt.left;
				}
				else {
					pt[0][1].x -= 10;
				}
				if (pt[0][1].y - 10 < rt.top) {
					pt[0][1].y = rt.top;
				}
				else {
					pt[0][1].y -= 10;
				}
			}
		}
		else if (wParam == '1') {
			count = 1;
		}
		else if (wParam == '2') {
			count = 2;
			for (int i = 0; i < count; ++i) {
				if (i != 0) {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = pt[i - 1][1].x + pt[i - 1][0].x;
					pt[i][0].y = 0;
				}
				else {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = 0;
					pt[i][0].y = 0;
				}
			}
		}
		else if (wParam == '3') {
			count = 3;
			for (int i = 0; i < count; ++i) {
				if (i != 0) {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = pt[i - 1][1].x + pt[i - 1][0].x;
					pt[i][0].y = 0;
				}
				else {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = 0;
					pt[i][0].y = 0;
				}
			}
		}
		else if (wParam == '4') {
			count = 4;
			for (int i = 0; i < count; ++i) {
				if (i != 0) {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = pt[i - 1][1].x + pt[i - 1][0].x;
					pt[i][0].y = 0;
				}
				else {
					pt[i][1].x = rt.right / count;
					pt[i][1].y = rt.bottom;
					pt[i][0].x = 0;
					pt[i][0].y = 0;
				}
			}
		}
		else if (wParam == 'r') {
			reverse = !reverse;
		}
		else if (wParam == 'q') {
			PostQuitMessage(0);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
		break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		for (int i = 0; i < count; ++i) {
			if (x >= pt[i][0].x && x <= pt[i][1].x + pt[i][0].x && y >= pt[i][0].y && y <= pt[i][1].y) {
				choose = i + 1;
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