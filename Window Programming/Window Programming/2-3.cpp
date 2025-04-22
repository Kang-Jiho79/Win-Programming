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
	int n, m, k;
	int r[2], g[2], b[2];
	TCHAR str1[100] = L"";
	TCHAR str2[100] = L"";
	bool check = true;

	switch (iMessage) {
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		do {
			n = rand() % 11;
			m = rand() % 11;
			k = rand() % 27;
		} while (n == 0 || n == 1 || m == 0 || m == 1 || k == 0 || k == 1);
		for (int i = 0; i < k; i++) {
			str1[i] = i + 'a';
			str2[i] = i + 'A';
		}
		str1[k] = '\0';
		str2[k] = '\0';
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				RECT rect;
				rect.left = 0 + (800 / n * j);
				rect.right = rect.left + 80;
				rect.top = 0 + (600 / m * i);
				rect.bottom = rect.top + 200;
				do {
					for (int l = 0; l < 2; l++) {
						r[l] = rand() % 255;
						g[l] = rand() % 255;
						b[l] = rand() % 255;
					}
				} while (r[0] <= r[1] + 100 && r[0] >= r[1] - 100 && g[0] <= g[1] + 100 && g[0] >= g[1] - 100 && b[0] <= b[1] + 100 && b[0] >= b[1] - 100);
				SetTextColor(hDC, RGB(r[0], g[0], b[0]));
				SetBkColor(hDC, RGB(r[1], g[1], b[1]));
				if (check) {
					DrawText(hDC, str1, _tcslen(str1), &rect, DT_WORDBREAK | DT_EDITCONTROL);
					check = false;
				}
				else {
					DrawText(hDC, str2, _tcslen(str2), &rect, DT_WORDBREAK | DT_EDITCONTROL);
					check = true;
				}
			}
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