#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

TCHAR str[100] = L"\0";
TCHAR s[4] = L"\0";
int count;
int x, y, n, r[2], g[2], b[2];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// �����Լ� 
{
	srand(time(NULL));
	x = rand() % 600;
	y = rand() % 400;
	n = rand() % 200;
	count = rand() % 15 + 5;
	for (int i = 0; i < 2; i++) {
		r[i] = rand() % 255;
		g[i] = rand() % 255;
		b[i] = rand() % 255;
	}
	wsprintf(s, L"%d", n);
	for (int i = 0; i < count; i++) {
		_tcscat(str, s);
	}
	str[count] = '\0';

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




LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;




	switch (iMessage) {
	case WM_CHAR:
		if (wParam == 'q') {
			PostQuitMessage(0);
			return 0;
		}
		break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN) {
			srand(time(NULL));
			x = rand() % 600;
			y = rand() % 400;
			n = rand() % 200;
			count = rand() % 15 + 5;
			for (int i = 0; i < 4; i++)
				s[i] = '\0';
			for (int i = 0; i < 2; i++) {
				r[i] = rand() % 255;
				g[i] = rand() % 255;
				b[i] = rand() % 255;
			}
			wsprintf(s, L"%d", n);
			for (int i = 0; i < 100; i++)
				str[i] = '\0';
			for (int i = 0; i < count; i++) {
				_tcscat(str, s);
			}
			str[(count * _tcslen(s)) + 1] = '\0';
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetTextColor(hDC, RGB(r[0], g[0], b[0]));
		SetBkColor(hDC, RGB(r[1], g[1], b[0]));
		for (int i = 0; i < count; i++) {
			TextOut(hDC, x, y + (20 * (i + 1)), str, _tcsclen(str));
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}