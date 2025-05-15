#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "resource1.h"

#define maxcell 5

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU2);	//�޴� �̸�
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
	POINT		picnum;		// ���° �׸�����
	bool		blank;		// ����ִ���
};

Cell cell[maxcell][maxcell];

HBITMAP hBitmap;
POINT   ptLT;				// ������
POINT   ptRB;				// ��������
POINT	choose = { 0,0 };	// ������ cell
POINT	dis = { 0,0 };
int		piccut = 3;			// ��������
int		movecount = 0;
bool	start = false;		// ������ �����ߴ���
bool	entire = false;		// �׸� ��ü���
bool	v = false;			// ���γ�����
bool	h = false;			// ���γ�����
bool	reverse = false;	// ����
bool	act = false;		// ���콺 �巡��������
bool	movecell = false;	// cell�� �����̴���

void swap(Cell& c1, Cell& c2)
{
	POINT temp1 = c1.picnum;
	c1.picnum = c2.picnum;
	c2.picnum = temp1;
	bool temp2 = c1.blank;
	c1.blank = c2.blank;
	c2.blank = temp2;
}


void setting(RECT rt)
{
	srand(time(NULL));
	if (v) {
		for (int x = 0; x < piccut; ++x) {
			cell[0][x].pt = { x * (rt.right / piccut),0 };
			cell[0][x].picnum = { x,0 };
			cell[0][x].blank = false;
		}
		for (int x = 0; x < piccut; ++x) {
			int i = rand() % piccut;
			swap(cell[0][x], cell[0][i]);
		}
	}
	else if (h) {
		for (int y = 0; y < piccut; ++y) {
			cell[0][y].pt = { 0,y * (rt.right / piccut)};
			cell[0][y].picnum = { 0,y };
			cell[0][y].blank = false;
		}
		for (int y = 0; y < piccut; ++y) {
			int i = rand() % piccut;
			swap(cell[0][y], cell[0][i]);
		}
	}
	else {
		int pic;
		for (int y = 0; y < piccut; ++y) {
			for (int x = 0; x < piccut; ++x) {
				cell[y][x].pt = { x * (rt.right / piccut),y * (rt.bottom / piccut) };
				cell[y][x].picnum = { x,y };
				cell[y][x].blank = false;
			}
		}
		for (int y = 0; y < piccut; ++y) {
			for (int x = 0; x < piccut; ++x) {
				int i = rand() % piccut;
				int j = rand() % piccut;
				swap(cell[y][x], cell[i][j]);
			}
		}
		int i = rand() % piccut;
		int j = rand() % piccut;
		cell[i][j].blank = true;
	}
}

// �� ����
void draw(HDC hdc, HDC mdc, RECT rt, BITMAP bmp, Cell cell)
{
	if (cell.blank) return;
	if (!reverse) {
		if (v) {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right / piccut, rt.bottom, mdc, cell.picnum.x * (bmp.bmWidth / piccut), 0, bmp.bmWidth / piccut, bmp.bmHeight, SRCCOPY);
		}
		else if (h) {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right, rt.bottom/piccut, mdc, 0, cell.picnum.y * (bmp.bmHeight/piccut), bmp.bmWidth, bmp.bmHeight / piccut, SRCCOPY);
		}
		else {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right/piccut, rt.bottom / piccut, mdc, cell.picnum.x * (bmp.bmWidth / piccut), cell.picnum.y * (bmp.bmHeight / piccut), bmp.bmWidth/piccut, bmp.bmHeight / piccut, SRCCOPY);
		}
	}
	else {
		if (v) {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right / piccut, rt.bottom, mdc, cell.picnum.x * (bmp.bmWidth / piccut), 0, bmp.bmWidth / piccut, bmp.bmHeight, NOTSRCCOPY);
		}
		else if (h) {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right, rt.bottom / piccut, mdc, 0, cell.picnum.y * (bmp.bmHeight / piccut), bmp.bmWidth, bmp.bmHeight / piccut, NOTSRCCOPY);
		}
		else {
			StretchBlt(hdc, cell.pt.x, cell.pt.y, rt.right / piccut, rt.bottom / piccut, mdc, cell.picnum.x * (bmp.bmWidth / piccut), cell.picnum.y * (bmp.bmHeight / piccut), bmp.bmWidth / piccut, bmp.bmHeight / piccut, NOTSRCCOPY);
		}
	}
}

bool endcheck() {
	if (!start) return false;
	if (v) {
		for (int i = 0; i < piccut; ++i)
			if (cell[0][i].picnum.x != i)
				return false;
	}
	else if (h) {
		for (int i = 0; i < piccut; ++i)
			if (cell[0][i].picnum.y != i)
				return false;
	}
	else {
		for (int i = 0; i < piccut; ++i)
			for (int j = 0; j < piccut; ++j)
				if (cell[i][j].picnum.x != j || cell[i][j].picnum.y != i)
					return false;
	}
	return true;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	BITMAP bmp;
	HDC hDC, mDC;
	
	RECT rt;
	GetClientRect(hWnd, &rt);
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 100, NULL);
		hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		SelectObject(mDC, (HBITMAP)hBitmap);
		if (start) {
			if (entire) {
				if (reverse)
					StretchBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, bmp.bmWidth, bmp.bmHeight, NOTSRCCOPY);
				else
					StretchBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			}
			else {
				if (v||h) {
					for (int i = 0; i < piccut; ++i)
						draw(hDC, mDC, rt, bmp, cell[0][i]);
					if (act)
						draw(hDC, mDC, rt, bmp, cell[choose.y][choose.x]);
				}
				else {
					for (int i = 0; i < piccut; ++i)
						for (int j = 0; j < piccut; ++j)
							draw(hDC, mDC, rt, bmp, cell[i][j]);
				}
			}
		}
		if (endcheck()) {
			if (MessageBox(hWnd, L"���� �� ����", L"�˸�", MB_ICONEXCLAMATION | MB_OK) == IDOK) {
				PostQuitMessage(0);
			}
		}
		DeleteDC(mDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CHAR:
		if (wParam == 'q') {
			PostQuitMessage(0);
		}
		else if (!movecell) {
			if (wParam == 's') {
				start = true;
				setting(rt);
			}
			else if (wParam == 'f') {
				entire = !entire;
			}
			else if (wParam == 'v') {
				v = !v;
				h = false;
				setting(rt);
			}
			else if (wParam == 'h') {
				h = !h;
				v = false;
				setting(rt);
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_COMMAND:
		if (!movecell) {
			switch (LOWORD(wParam))
			{
			case ID_PICTURE_1:
				hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
				break;
			case ID_PICTURE_2:
				hBitmap = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
				break;
			case ID_PICTUREDIVIDE_3:
				piccut = 3;
				setting(rt);
				break;
			case ID_PICTUREDIVIDE_4:
				piccut = 4;
				setting(rt);
				break;
			case ID_PICTUREDIVIDE_5:
				piccut = 5;
				setting(rt);
				break;
			case ID_GAME_START:
				start = true;
				setting(rt);
				break;
			case ID_GAME_ENTIREPICTURE:
				entire = !entire;
				break;
			case ID_GAME_REVERSE:
				reverse = !reverse;
				break;
			case ID_GAME_END:
				PostQuitMessage(0);
				break;
			default:
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
	{
		if (start && !movecell && !entire) {
			ptLT.x = LOWORD(lParam);
			ptLT.y = HIWORD(lParam);
			if (v) {
				for (int i = 0; i < piccut; ++i) {
					if (ptLT.x >= cell[0][i].pt.x && ptLT.x <= cell[0][i].pt.x + rt.right / piccut) {
						choose = { i,0 };
						act = true;
					}
				}
			}
			else if (h) {
				for (int i = 0; i < piccut; ++i) {
					if (ptLT.y >= cell[0][i].pt.y && ptLT.y <= cell[0][i].pt.y + rt.bottom / piccut) {
						choose = { i,0 };
						act = true;
					}
				}
			}
			else
				act = true;
			InvalidateRect(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (start && !movecell && act) {
			ptRB.x = LOWORD(lParam);
			ptRB.y = HIWORD(lParam);
			if (v || h) {
				int x = ptRB.x - ptLT.x;
				int y = ptRB.y - ptLT.y;
				cell[choose.y][choose.x].pt.x += x;
				cell[choose.y][choose.x].pt.y += y;
				ptLT.x = ptRB.x;
				ptLT.y = ptRB.y;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (start && !movecell&& act) {
			act = false;
			ptRB.x = LOWORD(lParam);
			ptRB.y = HIWORD(lParam);
			if (v) {
				bool change = false;
				for (int i = 0; i < piccut; ++i) {
					if (choose.x != i && ptRB.x >= cell[0][i].pt.x && ptRB.x <= cell[0][i].pt.x + rt.right / piccut) {
						change = true;
						cell[choose.y][choose.x].pt = { choose.x * rt.right / piccut,0 };
						swap(cell[0][i], cell[choose.y][choose.x]);
					}
				}
				if (!change) {
					cell[choose.y][choose.x].pt = { choose.x * rt.right / piccut,0 };
				}
			}
			else if (h) {
				bool change = false;
				for (int i = 0; i < piccut; ++i) {
					if (choose.x != i && ptRB.y >= cell[0][i].pt.y && ptRB.y <= cell[0][i].pt.y + rt.bottom / piccut) {
						change = true;
						cell[choose.y][choose.x].pt = { 0,choose.x * rt.bottom / piccut };
						swap(cell[0][i], cell[choose.y][choose.x]);
					}
				}
				if (!change) {
					cell[choose.y][choose.x].pt = { 0,choose.x * rt.bottom / piccut };
				}
			}
			else {
				if (ptRB.x - ptLT.x >= 100) {
					for (int y = 0; y < piccut; ++y)
						for (int x = 0; x < piccut; ++x)
							if (cell[y][x].blank == true && x != 0) {
								movecell = true;
								dis = { 1,0 };
							}
				}
				else if (ptRB.x - ptLT.x <= -100) {
					for (int y = 0; y < piccut; ++y)
						for (int x = 0; x < piccut; ++x)
							if (cell[y][x].blank == true && x != piccut - 1) {
								movecell = true;
								dis = { -1,0 };
							}
				}
				else if (ptRB.y - ptLT.y >= 100) {
					for (int y = 0; y < piccut; ++y)
						for (int x = 0; x < piccut; ++x)
							if (cell[y][x].blank == true && y != 0) {
								movecell = true;
								dis = { 0,1 };
							}
				}
				else if (ptRB.y - ptLT.y <= -100) {
					for (int y = 0; y < piccut; ++y)
						for (int x = 0; x < piccut; ++x)
							if (cell[y][x].blank == true && y != piccut - 1) {
								movecell = true;
								dis = { 0,-1 };
							}
				}

			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (start && !movecell && !entire) {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (v) {
				for (int i = 0; i < piccut; ++i) {
					if (x >= cell[0][i].pt.x && x <= cell[0][i].pt.x + (rt.right / piccut)) 
						cell[0][i].blank = !cell[0][i].blank;
				}
			}
			else if (h) {
				for (int i = 0; i < piccut; ++i) {
					if (y >= cell[0][i].pt.y && y <= cell[0][i].pt.y + (rt.right / piccut))
						cell[0][i].blank = !cell[0][i].blank;
				}
			}
			else {
				for (int i = 0; i < piccut; ++i)
					for (int j = 0; j < piccut; ++j)
						if (x >= cell[i][j].pt.x && x <= cell[i][j].pt.x + (rt.right / piccut) && y >= cell[i][j].pt.y && y <= cell[i][j].pt.y + (rt.bottom / piccut))
							cell[i][j].blank = !cell[i][j].blank;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}

	}
	break;
	case WM_TIMER:
	{
		if (movecell) {
			for (int i = 0; i < 30; ++i) {
				movecount++;
				for (int y = 0; y < piccut; ++y)
					for (int x = 0; x < piccut; ++x)
						if (cell[y][x].blank == true && y - dis.y >= 0 && y - dis.y < piccut && x - dis.x >= 0 && x - dis.x < piccut && cell[y - dis.y][x - dis.x].blank == false) {
							if (dis.x == 0) {
								cell[y - dis.y][x - dis.x].pt.y += dis.y;
								if (cell[y - dis.y][x - dis.x].pt.y == y * rt.bottom / piccut) {
									cell[y - dis.y][x - dis.x].pt = { (x - dis.x) * (rt.right / piccut),(y - dis.y) * (rt.bottom / piccut) };
									swap(cell[y - dis.y][x - dis.x], cell[y][x]);
									movecell = false;
								}
							}
							else {
								cell[y - dis.y][x - dis.x].pt.x += dis.x;
								if (cell[y - dis.y][x - dis.x].pt.x == x * rt.right / piccut) {
									cell[y - dis.y][x - dis.x].pt = { (x - dis.x) * (rt.right / piccut),(y - dis.y) * (rt.bottom / piccut) };
									swap(cell[y - dis.y][x - dis.x], cell[y][x]);
									movecell = false;
								}
							}
							
						}
				if (!movecell || movecount == 1000) {
					movecount = 0;
					movecell = false;
					break;
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
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