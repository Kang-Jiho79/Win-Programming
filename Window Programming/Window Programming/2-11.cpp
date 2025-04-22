#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define xdistance 250
#define ydistance 200

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

// 200 x 150 �簢���׸���
void quadrangle(HDC hdc, int x, int y)
{
	Rectangle(hdc, x - 100, y - 75, x + 100, y + 75);
}

// 100 x 100 �� �׸���
void circle(HDC hdc, int x, int y)
{
	Ellipse(hdc, x - 50, y - 50, x + 50, y + 50);
}
// 200 x 100 Ÿ�� �׸���
void oval(HDC hdc, int x, int y)
{
	Ellipse(hdc, x - 75, y - 50, x + 75, y + 50);
}

// 100 x 100 �𷹽ð� �׸���
void sandglass(HDC hdc, int x, int y)
{
	POINT point[7] = { {x,y},{x - 50,y - 50},{x + 50,y - 50},{x,y},{x + 50,y + 50},{x - 50,y + 50},{x,y} };
	Polygon(hdc, point, 7);
}
// 100 x 100 �����ִ� �𷹽ð�׸���
void liedownsandglass(HDC hdc, int x, int y)
{
	POINT point[7] = { {x,y},{x - 50,y - 50},{x - 50,y + 50},{x,y},{x + 50,y - 50},{x + 50,y + 50},{x,y} };
	Polygon(hdc, point, 7);
}

// 100 x 100 ������ �׸���
void pentagon(HDC hdc, int x, int y)
{
	double pi = 3.14;
	int r = 60;
	POINT point[5];
	for (int i = 0; i < 5; i++) {
		double angle = -pi / 2 + i * (2 * pi / 5);
		point[i].x = (x + r * cos(angle));
		point[i].y = (y + r * sin(angle));
	}
	Polygon(hdc, point, 5);
}
// 100 x 100 �Ųٷε� ������ �׸���
void reversepentagon(HDC hdc, int x, int y)
{
	double pi = 3.14;
	int r = 60;
	POINT point[5];
	for (int i = 0; i < 5; i++) {
		double angle = pi / 2 + i * (2 * pi / 5);
		point[i].x = (x + r * cos(angle));
		point[i].y = (y + r * sin(angle));
	}
	Polygon(hdc, point, 5);
}

// 100 x 100 ���� �׸���
void pie(HDC hdc, int x, int y)
{
	Pie(hdc, x - 50, y - 50, x + 50, y + 50, x, y - 50, x + 50, y);
}
// 100 x 100 ������ ���̺κ� �׸���
void remainpie(HDC hdc, int x, int y)
{
	Pie(hdc, x - 50, y - 50, x + 50, y + 50, x + 50, y, x, y - 50);
}

int present = 4; // ���� �߾��� �׷����ִ� ���� 0 : ��/ 1 : �𷹽ð�/ 2 : ������/ 3 : ����
bool pushcheck[5] = { false }; // �����ִ��� Ȯ��
int turn[4] = { 0,1,2,3 };



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HPEN hDefPen;
	HBRUSH hDefBrush, hRedBrush, hBlueBrush, hGreenBrush, hYellowBrush, hTurquoiseBrush;
	RECT Now;

	GetClientRect(hWnd, &Now);
	int x = (Now.left + Now.right) / 2, y = (Now.top + Now.bottom) / 2;
	POINT point[4] = { {x - xdistance , y}, {x,y - ydistance}, {x + xdistance,y}, { x,y + ydistance } };
	switch (iMessage) {

	case WM_KEYDOWN:
	{
		if (wParam == 'Q') {
			PostQuitMessage(0);
			break;
		}
		else if (wParam == 'C') {
			pushcheck[0] = true;
			present = 0;
		}
		else if (wParam == 'S') {
			pushcheck[1] = true;
			present = 1;
		}
		else if (wParam == 'P') {
			pushcheck[2] = true;
			present = 2;
		}
		else if (wParam == 'E') {
			pushcheck[3] = true;
			present = 3;
		}
		else if (wParam == VK_LEFT) {
			for (int i = 0; i < 4; i++) {
				if (turn[i] == 0)
					turn[i] = 3;
				else
					turn[i]--;
				if (turn[i] == 0)
					present = i;
			}
		}
		else if (wParam == VK_RIGHT) {
			for (int i = 0; i < 4; i++) {
				if (turn[i] == 3)
					turn[i] = 0;
				else
					turn[i]++;
				if (turn[i] == 0)
					present = i;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_KEYUP:
	{
		if (wParam == 'C') {
			pushcheck[0] = false;
		}
		else if (wParam == 'S') {
			pushcheck[1] = false;
		}
		else if (wParam == 'P') {
			pushcheck[2] = false;
		}
		else if (wParam == 'E') {
			pushcheck[3] = false;
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
		hBlueBrush = CreateSolidBrush(RGB(0, 0, 255));
		hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
		hYellowBrush = CreateSolidBrush(RGB(255, 255, 0));
		hTurquoiseBrush = CreateSolidBrush(RGB(0, 255, 255));
		quadrangle(hDC, x, y);
		if (pushcheck[0])
			hDefBrush = (HBRUSH)SelectObject(hDC, hTurquoiseBrush);
		else
			hDefBrush = (HBRUSH)SelectObject(hDC, hBlueBrush);
		circle(hDC, point[turn[0]].x, point[turn[0]].y);
		if (present == 0)
			oval(hDC, x, y);
		SelectObject(hDC, hDefBrush);

		if (pushcheck[1])
			hDefBrush = (HBRUSH)SelectObject(hDC, hTurquoiseBrush);
		else
			hDefBrush = (HBRUSH)SelectObject(hDC, hRedBrush);
		sandglass(hDC, point[turn[1]].x, point[turn[1]].y);
		if (present == 1)
			liedownsandglass(hDC, x, y);
		SelectObject(hDC, hDefBrush);


		if (pushcheck[2])
			hDefBrush = (HBRUSH)SelectObject(hDC, hTurquoiseBrush);
		else
			hDefBrush = (HBRUSH)SelectObject(hDC, hYellowBrush);
		pentagon(hDC, point[turn[2]].x, point[turn[2]].y);
		if (present == 2)
			reversepentagon(hDC, x, y);
		SelectObject(hDC, hDefBrush);

		if (pushcheck[3])
			hDefBrush = (HBRUSH)SelectObject(hDC, hTurquoiseBrush);
		else
			hDefBrush = (HBRUSH)SelectObject(hDC, hGreenBrush);
		pie(hDC, point[turn[3]].x, point[turn[3]].y);
		if (present == 3)
			remainpie(hDC, x, y);
		SelectObject(hDC, hDefBrush);

		DeleteObject(hRedBrush);
		DeleteObject(hBlueBrush);
		DeleteObject(hGreenBrush);
		DeleteObject(hYellowBrush);
		DeleteObject(hTurquoiseBrush);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
