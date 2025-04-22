#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

TCHAR str[10][81] = { L"\0" };
int count[10] = { 0 };
int x[10] = { 400 }, y[10] = { 300 };
int entercount = 0;
int r[10], g[10], b[10];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// �����Լ� 
{
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		r[i] = rand() % 255;
		g[i] = rand() % 255;
		b[i] = rand() % 255;
	}
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
	{
		if (wParam == VK_RETURN) {
			entercount++;
			x[entercount] = x[entercount - 1] - 10;
			y[entercount] = y[entercount - 1] + 20;
		}
		else if (count[entercount] <= 79) {
			str[entercount][count[entercount]++] = wParam;
		}
		str[entercount][count[entercount]] = '\0';
		InvalidateRect(hWnd, NULL, false);

	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_F1) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < count[i]; j++)
					str[i][j] = '\0';
				count[i] = 0;
				x[i] = 400;
				y[i] = 300;
				entercount = 0;
				r[i] = rand() % 255;
				g[i] = rand() % 255;
				b[i] = rand() % 255;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
	}
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		if (entercount == 0) {
			SetTextColor(hDC, RGB(r[entercount], g[entercount], b[entercount]));
			TextOut(hDC, x[entercount], y[entercount], str[entercount], _tcsclen(str[entercount]));
		}
		else {
			SetTextColor(hDC, RGB(r[entercount], g[entercount], b[entercount]));
			TextOut(hDC, x[entercount], y[entercount], str[entercount], _tcsclen(str[entercount]));
			TextOut(hDC, x[entercount], y[entercount] - (entercount * 40), str[entercount], _tcsclen(str[entercount]));
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}