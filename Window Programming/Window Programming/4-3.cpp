#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>

#include "resource1.h"

#define tablecount 10
#define cellsize 40
#define size 15

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);	//�޴� �̸�
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

struct Cell
{
	POINT		pt;
	int			type;	// 0:������� 1:���� 2:������ 3:����
	int			pie;	// 0:���̾ƴ� 1:���� 2:��� 3:���� 4:�Ͽ�
	COLORREF	color;	// ���̻���
	bool		cover;	// �����ִ���
	bool		collect;// �� �����ִ���
};

Cell		cell[tablecount][tablecount];
COLORREF	colors[5] = { RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),RGB(255,255,0),RGB(255,0,255) };
int			collectpie = 0;
int			hintcount = 0;
bool		game = false;
bool		hint = false;
bool		score = false;

void gamesetting()
{
	srand(time(NULL));
	int pie = 0, item = 0, bomb = 0;
	for (int i = 0; i < tablecount; i++) {
		for (int j = 0; j < tablecount; j++) {
			cell[i][j].pt.x = j;
			cell[i][j].pt.y = i;
			cell[i][j].type = 0;
			cell[i][j].pie = 0;
			cell[i][j].cover = true;
		}
	}
	while (pie < 20 || item < 10||bomb <20) {
		if (pie < 20) {
			int x = rand() % tablecount, y = rand() % tablecount;
			if (cell[y][x].type == 0) {
				pie++;
				cell[y][x].type = 1;
				cell[y][x].pie = pie % 4 + 1;
				cell[y][x].color = (pie == 20 ? colors[0] : colors[pie / 4]);
			}
		}
		if (item < 10) {
			int x = rand() % tablecount, y = rand() % tablecount;
			if (cell[y][x].type == 0) {
				item++;
				cell[y][x].type = 2;
			}
		}
		if (bomb < 20) {
			int x = rand() % tablecount, y = rand() % tablecount;
			if (cell[y][x].type == 0) {
				bomb++;
				cell[y][x].type = 3;
			}
		}
	}
}


// �� �׸���
void drawtable(HDC hdc)
{
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}
void upleftpie(HDC hdc, POINT pt)
{
	Pie(hdc, pt.x - size, pt.y - size, pt.x + size, pt.y + size, pt.x, pt.y - size, pt.x - size, pt.y);
}
void uprightpie(HDC hdc, POINT pt)
{
	Pie(hdc, pt.x - size, pt.y - size, pt.x + size, pt.y + size, pt.x + size, pt.y, pt.x, pt.y - size);
}
void downleftpie(HDC hdc, POINT pt)
{
	Pie(hdc, pt.x - size, pt.y - size, pt.x + size, pt.y + size, pt.x - size, pt.y, pt.x, pt.y + size);
}
void downrightpie(HDC hdc, POINT pt)
{
	Pie(hdc, pt.x - size, pt.y - size, pt.x + size, pt.y + size, pt.x, pt.y + size, pt.x + size, pt.y);
}
void drawitem(HDC hdc, POINT pt) 
{
	POINT point[4] = { {pt.x - size,pt.y},{pt.x,pt.y - size},{pt.x + size,pt.y},{pt.x,pt.y + size} };
	Polygon(hdc, point, 4);
}
void drawcell(HDC hdc, Cell c)
{
	if (!hint) {
		if (c.cover) return;
	}
	POINT pt = { (c.pt.x * cellsize) + cellsize / 2,(c.pt.y * cellsize) + cellsize / 2 };
	if (c.type == 0) {
		HPEN pen = CreatePen(BS_SOLID, 1, RGB(255, 0, 0));
		HPEN oldpen = (HPEN)SelectObject(hdc, pen);
		TextOut(hdc, pt.x, pt.y, L"1", 1);
		SelectObject(hdc, oldpen);
		DeleteObject(pen);
	}
	else if (c.type == 1) {
		HBRUSH brush = CreateSolidBrush(c.color);
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
		if (c.pie == 1) {
			upleftpie(hdc, pt);
		}
		else if (c.pie == 2) {
			uprightpie(hdc, pt);
		}
		else if (c.pie == 3) {
			downleftpie(hdc, pt);
		}
		else if (c.pie == 4) {
			downrightpie(hdc, pt);
		}
		
	}
	else if (c.type == 2) {
		HBRUSH brush = CreateSolidBrush(RGB(0, 255, 255));
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
		drawitem(hdc, pt);
		SelectObject(hdc, oldbrush);
		DeleteObject(brush);
	}
	else if (c.type == 3) {
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, pt.x, pt.y, L"B", 1);
		SetTextColor(hdc, RGB(0, 0, 0));
	}
}
void checkpie()
{
	int check = 0;
	for (int i = 0; i < 5; ++i) {
		for (int y = 0; y<tablecount;++y)
			for (int x = 0; x < tablecount; ++x)
				if (cell[y][x].color == colors[i] && !cell[y][x].cover && !cell[y][x].collect) {
					check++;
					if (check == 4) {
						collectpie++;
						for (int j = 0; j < tablecount; ++j)
							for (int k = 0; k < tablecount; ++k)
								if (cell[j][k].color == colors[i])
									cell[j][k].collect = true;
					}
				}
		check = 0;
	}
}
void useitem()
{
	for (int y = 0; y < tablecount; ++y)
		for (int x = 0; x < tablecount; ++x)
			if (cell[y][x].type == 1 && !cell[y][x].cover) {
				for (int i = 0; i < tablecount; ++i)
					for (int j = 0; j < tablecount; ++j)
						if (cell[i][j].type == 1 && cell[i][j].cover && cell[i][j].color == cell[y][x].color)
							cell[i][j].cover = false;
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
		gamesetting();
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
		if (game) {
			drawtable(mDC);
			for (int i = 0; i < tablecount; i++) {
				for (int j = 0; j < tablecount; j++)
					drawcell(mDC, cell[i][j]);
			}
			if (score) {
				TCHAR ch[10];
				wsprintf(ch, L"���� : %d", collectpie);
				TextOut(mDC, 0, tablecount * cellsize + 20, ch, _tcslen(ch));
			}
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_GAMESTART:
			game = true;
			break;
		case ID_GAMEEND:
			PostQuitMessage(0);
			break;
		case ID_HINT:
			hint = true;
			break;
		case ID_SCORE:
			score = true;
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER: {	
		if (hint) {
			hintcount++;
			if (hintcount == 3) {
				hint = false;
				hintcount = 0;
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
		
		
	}
		break;
	case WM_LBUTTONDOWN:
	{
		if (game && !hint) {
			int y = HIWORD(lParam);
			int x = LOWORD(lParam);
			for (int i = 0; i < tablecount; i++) {
				for (int j = 0; j < tablecount; j++) {
					POINT pt = { (cell[i][j].pt.x * cellsize) + cellsize / 2,(cell[i][j].pt.y * cellsize) + cellsize / 2 };
					if (x >= pt.x - cellsize / 2 && x <= pt.x + cellsize / 2 && y >= pt.y - cellsize / 2 && y <= pt.y + cellsize / 2) {
						cell[i][j].cover = false;
						if (cell[i][j].type == 1) {
							checkpie();
						}
						else if (cell[i][j].type == 2) {
							useitem();
							checkpie();
						}
						else if (cell[i][j].type == 3)
							PostQuitMessage(0);
					}
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