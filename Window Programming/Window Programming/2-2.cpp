#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	int x[11];
	int y[11];
	TCHAR str[100];

	switch (iMessage) {
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		x[0] = 400;
		y[0] = 300;
		wsprintf(str, L"%d: (%d, %d)", 0, x[0], y[0]);
		TextOut(hDC, x[0], y[0], str, _tcslen(str));
		for (int i = 1; i < 11; i++) {
			bool check = false;
			do {
				check = false;
				x[i] = rand() % 701;
				y[i] = rand() % 551;
				for (int j = 0; j < i; j++) {
					if (x[i] >= x[j] && x[i] <= x[j] + 100 && x[i] + 100 >= x[j] && x[i] + 100 <= x[j] + 100 && y[i] >= y[j] && y[i] <= y[j] + 50 && y[i] + 50 >= y[j] && y[i] + 50 <= y[j] + 50)
						check = true;
				}
			} while (check);
			wsprintf(str, L"%d: (%d, %d)", i, x[i], y[i]);
			TextOut(hDC, x[i], y[i], str, _tcslen(str));
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}