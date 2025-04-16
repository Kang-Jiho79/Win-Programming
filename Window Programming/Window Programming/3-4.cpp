//�ǽ� 3-4��
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 25
#define cellsize 25
#define maxcell 10

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


struct Cell {
	int playercheck; // 0 : �÷��̾�ƴ� 1 : ���� 2 : ���� 3 : �簢��
	int colortype;	// 0 : �� 1 : ���� 2 : �ʷ� 3 ; �Ķ�
	int x, y;
	int rotationcheck;
	COLORREF color;
};

bool startcheck = false;
Cell player;
Cell table[tablecount][tablecount];

COLORREF colors[4] = { RGB(255,255,255),RGB(255,0,0),RGB(0,255,0),RGB(0,0,255) };

// ���� ���� ����
void settinggame()
{
	srand(unsigned(time(NULL)));
	for (int i = 0; i < tablecount; i++)
		for (int j = 0; j < tablecount; j++) {
			table[i][j].playercheck = 0;
			table[i][j].colortype = 0;
			table[i][j].x = j;
			table[i][j].y = i;
			table[i][j].color = colors[0];
			table[i][j].rotationcheck = false;
		}
	player.playercheck = 0;
	player.colortype = 0;
	player.x = 14;
	player.y = 3;
	player.color = colors[0];
	player.rotationcheck = false;
}
// ���� ����
void startgame() {
	startcheck = true;
	int selectplayer = rand() % 3 + 1;
	player.playercheck = selectplayer;
	player.colortype = selectplayer;
	player.x = 14;
	player.y = 3;
	player.color = colors[selectplayer];
	player.rotationcheck = false;
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

// �÷��̾� ���߰��ϱ�
void playermovestop(Cell cell)
{
	switch (cell.playercheck)
	{
	case 1: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[cell.y][cell.x + i].colortype = cell.playercheck;
				table[cell.y][cell.x + i].color = colors[cell.playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[cell.y - i][cell.x].colortype = cell.playercheck;
				table[cell.y - i][cell.x].color = colors[cell.playercheck];
			}
		}
	}
		  break;
	case 2: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[cell.y - i][cell.x].colortype = cell.playercheck;
				table[cell.y - i][cell.x].color = colors[cell.playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[cell.y][cell.x + i].colortype = cell.playercheck;
				table[cell.y][cell.x + i].color = colors[cell.playercheck];
			}
		}
	}
		  break;
	case 3: {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				table[cell.y - i][cell.x + j].colortype = cell.playercheck;
				table[cell.y - i][cell.x + j].color = colors[cell.playercheck];
			}
	}
		  break;
	default:
		break;
	}
	int selectplayer = rand() % 3 + 1;
	cell.playercheck = selectplayer;
	cell.colortype = selectplayer;
	cell.x = 14;
	cell.y = 3;
	cell.color = colors[selectplayer];
	cell.rotationcheck = false;
}


// �÷��̾� ������
void MovePlayer(int dx, int dy, Cell cell) {
	int nx = cell.x + dx;
	int ny = cell.y + dy;
	if (nx < 0) return;
	else if (nx >= tablecount) return;
	else if (ny >= tablecount) {
		playermovestop(player);
		return;
	}
	switch (cell.playercheck)
	{
	case 1: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				if (table[ny][nx + i].colortype != 0) {
					if (dx == 0) {
						playermovestop(player);
						return;
					}
					else
						return;
				}
				table[cell.y][cell.x + i].colortype = 0;
				table[cell.y][cell.x + i].color = colors[0];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				if (table[ny - i][nx].colortype != 0) {
					if (dx == 0) {
						playermovestop(player);
						return;
					}
					else
						return;
				}
				table[cell.y - i][cell.x].colortype = 0;
				table[cell.y - i][cell.x].color = colors[0];
			}
		}
	}
		  break;
	case 2: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				if (table[ny - i][nx].colortype != 0) {
					if (dx == 0) {
						playermovestop(player);
						return;
					}
					else
						return;
				}
				table[cell.y - i][cell.x].colortype = 0;
				table[cell.y - i][cell.x].color = colors[0];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				if (table[ny][nx + i].colortype != 0) {
					if (dx == 0) {
						playermovestop(player);
						return;
					}
					else
						return;
				}
				table[cell.y][cell.x + i].colortype = 0;
				table[cell.y][cell.x + i].color = colors[0];
			}
		}
	}
		  break;
	case 3: {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				if (table[ny - i][nx + j].colortype != 0) {
					if (dx == 0) {
						playermovestop(player);
						return;
					}
					else
						return;
				}
				table[cell.y - i][cell.x + j].colortype = 0;
				table[cell.y - i][cell.x + j].color = colors[0];
			}
	}
		  break;
	default:
		break;
	}
	cell.x = nx;
	cell.y = ny;
}

// �簢�� �׸���
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

// �� ����
void drawshape(HDC hdc, Cell cell)
{
	int rx = cellsize / 2 + (cellsize * cell.x);
	int ry = cellsize / 2 + (cellsize * cell.y);
	HBRUSH brush = CreateSolidBrush(cell.color);
	SelectObject(hdc, brush);
	square(hdc, rx, ry, cellsize);
	DeleteObject(brush);

}
// �÷��̾� �׸���
void drawplayer(HDC hdc, Cell cell)
{
	if (cell.playercheck == 0)
		return;
	int rx = cellsize / 2 + (cellsize * cell.x);
	int ry = cellsize / 2 + (cellsize * cell.y);
	switch (cell.playercheck)
	{
	case 1: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[cell.y][cell.x + i].colortype = cell.playercheck;
				table[cell.y][cell.x + i].color = colors[cell.playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[cell.y - i][cell.x].colortype = cell.playercheck;
				table[cell.y - i][cell.x].color = colors[cell.playercheck];
			}
		}
	}
		  break;
	case 2: {
		if (cell.rotationcheck) {
			for (int i = 0; i < 4; i++) {
				table[cell.y - i][cell.x].colortype = cell.playercheck;
				table[cell.y - i][cell.x].color = colors[cell.playercheck];
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				table[cell.y][cell.x + i].colortype = cell.playercheck;
				table[cell.y][cell.x + i].color = colors[cell.playercheck];
			}
		}
	}
		  break;
	case 3: {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				table[cell.y - i][cell.x + j].colortype = cell.playercheck;
				table[cell.y - i][cell.x + j].color = colors[cell.playercheck];
			}
	}
		  break;
	default:
		break;
	}
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
		SetTimer(hWnd, 1, 1000, NULL);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		drawtable(hdc);
		drawplayer(hdc, player);
		for (int i = 0; i < tablecount; ++i)
			for (int j = 0; j < tablecount; j++) {
				drawshape(hdc, table[i][j]);
			}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:

		if (wParam == 'Q')
			PostQuitMessage(0);
		if (startcheck) {
			if (wParam == VK_UP) MovePlayer(0, -(player.y - 3), player);
			else if (wParam == VK_LEFT) MovePlayer(-1, 0, player);
			else if (wParam == VK_DOWN) MovePlayer(0, 1, player);
			else if (wParam == VK_RIGHT) MovePlayer(1, 0, player);
			else if (wParam == VK_RETURN) player.rotationcheck = ~player.rotationcheck;
		}
		else
			if (wParam == 'S') {
				startgame();
			}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER: {
		if (startcheck)
			if (player.playercheck != 0)
				MovePlayer(0, 1, player);
		InvalidateRect(hWnd, NULL, TRUE);
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
