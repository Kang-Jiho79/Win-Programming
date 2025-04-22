//�ǽ� 13��
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

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);	//������ ����� �Լ�

	ShowWindow(hWnd, nCMdShow);	//	������ ����
	UpdateWindow(hWnd);		// ������ ������Ʈ

	while (GetMessage(&Message, 0, 0, 0)) {		//	�޽��� ���� (�޽��� �޾ƿ��� ����) -  ����޽����� ���ö����� �ݺ�
		TranslateMessage(&Message);		//	�޽��� ����
		DispatchMessage(&Message);	//	�޽��� ó��
	}
	return Message.wParam;
}

struct Player {
	int x, y, speed, size;
	int movecheck;
	bool width, length;
};



Player player;

// ���� ���� ����
void settinggame()
{
	player.x = 20;
	player.y = 20;
	player.size = 20;
	player.speed = 5;
	player.movecheck = 0;
	player.width = true;
	player.length = true;
}


void MovePlayer(int dx, int dy) {
	player.x += dx;
	player.y += dy;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	static RECT rectView;
	switch (iMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rectView);
		SetTimer(hWnd, 1, 1000 * player.speed, NULL);
		settinggame();
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		circle(hDC, player.x, player.y, player.size);
		EndPaint(hWnd, &ps);
		break;
	case WM_CHAR:
		if (wParam == 'h')
			player.movecheck = 1;
		else if (wParam == 'v')
			player.movecheck = 2;
		else if (wParam == 'p')
			player.movecheck = 0;
		else if (wParam == '-') {
			if (player.speed > 1) {
				player.speed--;
			}
		}
		else if (wParam == '+') {
			if (player.speed < 10) {
				player.speed++;
			}
		}
		else if (wParam == 'q')
			PostQuitMessage(0);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		player.x = LOWORD(lParam);
		player.y = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		if (player.movecheck == 1) {
			if (player.x + player.size / 2 > rectView.right) {
				player.width = false;
				if (player.y + player.size > rectView.bottom)
					player.y = 20;
				else
					player.y += cellsize;
			}
			else if (player.x - player.size / 2 < 0) {
				player.width = true;
				if (player.y + player.size > rectView.bottom)
					player.y = 20;
				else
					player.y += cellsize;
			}
			if (player.width) { MovePlayer(player.speed, 0); }
			else { MovePlayer(-player.speed, 0); }
		}
		else if (player.movecheck == 2) {
			if (player.y + player.size / 2 > rectView.bottom) {
				player.length = false;
				if (player.x + player.size > rectView.right)
					player.x = 20;
				else
					player.x += cellsize;
			}
			else if (player.y - player.size / 2 < 0) {
				player.length = true;
				if (player.x + player.size > rectView.right)
					player.x = 20;
				else
					player.x += cellsize;
			}
			if (player.length) { MovePlayer(0, player.speed); }
			else { MovePlayer(0, -player.speed); }
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}