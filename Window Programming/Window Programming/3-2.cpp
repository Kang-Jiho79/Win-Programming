#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 25
#define cellsize 20
#define tailscount 20

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

struct Cell
{
	int x, y;			// ��ġ
	int size;			// ũ��
	COLORREF color;		// ��
};

struct Circle
{
	int x, y;			// ��ġ
	int px, py;			// ���� ��ġ
	int dx, dy;			// �̵� ����
	COLORREF color;		// ��
	int movetype;		// �̵�Ÿ��
	int prevtail;		// �տ� �پ��ִ� ������ ��ȣ
	int endtail;		// �ǵ� ������ ��ȣ
	int number;			// �������� ���°����
};

Circle player;
Cell cell[tailscount];
Circle tails[tailscount];
int linklist[tailscount];
int linkcount = 0;

// ���� �ʱ⼼��
void settinggame()
{
	srand(unsigned(time(NULL)));
	player.x = 0;
	player.y = 0;
	player.dx = 1;
	player.dy = 0;
	player.number = 0;
	player.color = RGB(rand() % 256, rand() % 256, rand() % 256);
	player.movetype = 0;
	player.endtail = -1;
	for (int i = 0; i < tailscount; i++)
	{
		tails[i].x = -1;
		tails[i].y = -1;
		tails[i].number = -1;
		tails[i].prevtail = -1;
		tails[i].endtail = i;
		cell[i].x = rand() % tablecount;
		cell[i].y = rand() % tablecount;
		cell[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
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


// �簢�� �׸���
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}
// �� �׸���
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

void tailset(Cell cell, Circle tail)
{
	tail.x = cell.x;
	tail.y = cell.y;
	tail.number = 0;
	tail.movetype = rand() % 5 + 1;
	tail.color = cell.color;
	switch (tail.movetype)
	{
	case 1: {
		tail.dx = 1;
		tail.dy = 0;
	}
		break;
	case 2: {
		tail.dx = 0;
		tail.dy = 1;
	}
		break;
	case 3: {
		tail.dx = 1;
		tail.dy = 1;
	}
		break;
	case 4: {
		tail.dx = 0;
		tail.dy = 0;
	}
		break;
	case 5: {
		tail.dx = 0;
		tail.dy = 0;
	}
		  break;
	default:
		break;
	}
	cell.x = -1;
	cell.y = -1;
}

void drawtail(HDC hdc, Circle c) {
	if (c.x == -1)
		return;
	HBRUSH brush = CreateSolidBrush(c.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	circle(hdc, c.x*cellsize + cellsize/2, c.y * cellsize + cellsize / 2, cellsize - (c.number + 1));
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

void drawcell(HDC hdc, Cell c) {
	if (c.x == -1)
		return;
	HBRUSH brush = CreateSolidBrush(c.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	square(hdc, c.x * cellsize + cellsize / 2, c.y * cellsize + cellsize / 2, cellsize/2);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

// ���� ������
void movetail()
{
	for (int i = 0; i < tailscount; i++) {
		if (tails[i].number == -1)
			return;
		tails[i].px = tails[i].x;
		tails[i].py = tails[i].y;
	}
	for (int i = 0; i < tailscount; i++) {
		if (tails[i].number == -1)
			return;
		if (tails[i].prevtail != -1) {
			if (tails[i].prevtail == 20) {
				tails[i].x = player.x;
				tails[i].y = player.y;
			}
			else {
				tails[i].x = tails[tails[i].prevtail].px;
				tails[i].y = tails[tails[i].prevtail].py;
			}
		}
		else {
			tails[i].x += tails[i].dx;
			tails[i].y += tails[i].dy;
		}
	}
}
// �÷��̾� ������
void moveplayer()
{
	int nx = player.x + player.dx;
	int ny = player.y + player.dy;
	if (player.dx != 0) {
		if (player.x == tablecount - 1 && player.dx == 1) {
			if (player.y == tablecount - 1) {
				player.y -= 1;
				player.dx = -1;
				player.dy = 0;
			}
			else {
				player.y += 1;
				player.dx = -1;
				player.dy = 0;
			}
			nx = player.x + player.dx + 1;
			ny = player.y + player.dy;
		}
		else if (player.x == 0 && player.dx == -1) {
			if (player.y == tablecount - 1) {
				player.y -= 1;
				player.dx = 1;
				player.dy = 0;
			}
			else {
				player.y += 1;
				player.dx = 1;
				player.dy = 0;
			}
			nx = player.x + player.dx-1;
			ny = player.y + player.dy;
		}
	}
	else {
		if (player.y == tablecount - 1 && player.dy == 1) {
			if (player.x == tablecount - 1) {
				player.x -= 1;
				player.dx = 0;
				player.dy = -1;
			}
			else {
				player.x += 1;
				player.dx = 0;
				player.dy = -1;
			}
			nx = player.x + player.dx;
			ny = player.y + player.dy+1;
		}
		else if (player.y == 0 && player.dy == -1) {
			if (player.x == tablecount - 1) {
				player.x -= 1;
				player.dx = 0;
				player.dy = 1;
			}
			else {
				player.x += 1;
				player.dx = 0;
				player.dy = 1;
			}
			nx = player.x + player.dx;
			ny = player.y + player.dy-1;
		}
	}

	movetail();
	player.x = nx;
	player.y = ny;
	for (int i = 0; i < tailscount; i++) {
		if (player.x == cell[i].x && player.y == cell[i].y) {
			tailset(cell[i], tails[i]);
			break;
		}
		else if(tails[i].number == -1){
			if (player.x == tails[i].x && player.y == tails[i].y) {
				if (player.endtail == -1) {
					tails[i].number = 0;
					tails[i].prevtail = 20;
					player.endtail = tails[i].endtail;
				}
				else {
					tails[i].number = tails[player.endtail].number + 1;
					tails[i].prevtail = player.endtail;
					player.endtail = tails[i].endtail;
				}
				if (tails[i].endtail != i) {
					int temp = tails[i].endtail;
					while (temp != i) {
						tails[temp].number += tails[i].number + 1;
						temp = tails[temp].prevtail;
					}
				}
				break;
			}
		}
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
		SetTimer(hWnd, 1, 100, NULL);
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
		for (int i = 0; i < tailscount; i++) {
			drawcell(mDC, cell[i]);
			drawtail(mDC, tails[i]);
		}
		drawtail(mDC, player);
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (wParam == 'Q')
			PostQuitMessage(0);
		else if (wParam == VK_UP) { player.dx = 0; player.dy = -1; }
		else if (wParam == VK_LEFT) { player.dx = -1; player.dy = 0; }
		else if (wParam == VK_RIGHT) { player.dx = 1; player.dy = 0; }
		else if (wParam == VK_DOWN) { player.dx = 0; player.dy = 1; }
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER: {
		moveplayer();
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