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

struct Player {
	int x, y;
};

struct Cell {
	bool check = false;
	int wordnumber = 0;
};

Player player;
Cell table[tablecount][tablecount];
TCHAR words[5][6] = { L"APPLE", L"WORLD", L"CLASS", L"TABLE", L"STUDY" };
int r;



// ���� ���� ����
void settinggame()
{
	srand((unsigned)time(0));
	r = rand() % 5;
	for (int i = 0; i < tablecount; i++)
		for (int j = 0; j < tablecount; j++) {
			table[i][j].check = false;
			table[i][j].wordnumber = 0;
		}
	for (int i = 0; i < 5; i++) {
		if (i == r) {
			table[1][8 + i].wordnumber = 0;
			table[1][8 + i].check = false;
		}
		else {
			table[1][8 + i].wordnumber = words[r][i] - 'A';
			table[1][8 + i].check = true;
		}
	}
	for (int i = 3; i < tablecount - 1; ++i) {
		for (int j = 1; j < tablecount - 1; ++j) {
			int r = rand() % 10;
			if (r == 0) {
				table[i][j].wordnumber = rand() % 26;
				table[i][j].check = true;
			}
		}
	}
	player.x = 10;
	player.y = 10;
}




void drawtable(HDC hdc)
{
	for (int i = 0; i < tablecount; i++) {
		for (int j = 0; j < tablecount; j++) {
			if (table[i][j].check == true) {
				TCHAR ch = 'A' + table[i][j].wordnumber;
				TextOut(hdc, ((j * cellsize) + cellsize / 2) - 5, ((i * cellsize) + cellsize / 2) - 5, &ch, 1);
			}
		}
	}
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}

int wordnumadd(int a, int b)
{
	return (a + b > 25 ? a + b - 25 : a + b + 1);
}

void MovePlayer(int dx, int dy) {
	int nx = player.x + dx;
	int ny = player.y + dy;
	if (nx < 0 || ny < 0 || nx >= tablecount || ny >= tablecount)
		return;
	if (table[ny][nx].check)
		if (nx + dx < 0 || ny + dy < 0 || nx + dx >= tablecount || ny + dy >= tablecount)
			return;
	player.x = nx;
	player.y = ny;

	if (table[ny][nx].check) {
		if (table[ny + dy][nx + dx].check) {
			table[ny + dy][nx + dx].wordnumber = wordnumadd(table[ny + dy][nx + dx].wordnumber, table[ny][nx].wordnumber);
		}
		else {
			table[ny + dy][nx + dx].wordnumber = table[ny][nx].wordnumber;
			table[ny + dy][nx + dx].check = true;
		}
		table[ny][nx].check = false;
		table[ny][nx].wordnumber = 0;
	}
}


// �� �׸���
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

void drawshape(HDC hdc, int x, int y)
{
	int rx = cellsize / 2 + (cellsize * x);
	int ry = cellsize / 2 + (cellsize * y);
	HBRUSH brush = CreateSolidBrush(RGB(0, 255, 255));
	SelectObject(hdc, brush);
	circle(hdc, rx, ry, cellsize);
	DeleteObject(brush);
}

bool endcheck()
{
	bool endchecking = true;
	for (int i = 0; i < 5; i++) {
		if (table[1][8 + i].wordnumber != words[r][i] - 'A' && !table[1][8 + i].check)
			endchecking = false;
	}
	return endchecking;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HPEN hDefPen;
	HBRUSH hDefBrush, hRedBrush, hBlueBrush, hGreenBrush, hYellowBrush, hTurquoiseBrush;
	RECT Now;
	switch (iMessage) {
	case WM_CREATE:
		settinggame();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		drawtable(hdc);
		drawshape(hdc, player.x, player.y);
		if (endcheck())
			TextOut(hdc, 0, cellsize * tablecount + 10, L"�ܾ� �ϼ�!", _tcslen(L"�ܾ� �ϼ�!"));
		else
			TextOut(hdc, 0, cellsize * tablecount + 10, words[r], _tcslen(words[r]));
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (!endcheck()) {
			if (wParam == VK_UP) MovePlayer(0, -1);
			else if (wParam == VK_LEFT) MovePlayer(-1, 0);
			else if (wParam == VK_DOWN) MovePlayer(0, 1);
			else if (wParam == VK_RIGHT) MovePlayer(1, 0);
		}
		if (wParam == 'Q')
			PostQuitMessage(0);
		else if (wParam == 'S')
			settinggame();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}