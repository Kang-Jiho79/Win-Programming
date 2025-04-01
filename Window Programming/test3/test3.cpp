﻿#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// 메인함수 
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;	//윈도우 클래스
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);	//	구조체의 크기
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//	윈도우 출력 스타일
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//	윈도우 메세지를 처리하는 윈도우 프로시저 함수 이름 (Long Pointer Funtion WndProc)
	WndClass.cbClsExtra = 0;	//	사용여분 메모리(class)
	WndClass.cbWndExtra = 0;	//	사용여분 메모리(Window)
	WndClass.hInstance = hInstance;		//응용 프로그램 ID
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// 실행파일의 쓰일 아이콘지정
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// 윈도우에서 쓰일 커서지정
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 윈도우의 배경색을 설정할수있다.
	WndClass.lpszMenuName = NULL;	//메뉴 이름
	WndClass.lpszClassName = IpszClass;		//	클래스의 이름
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//작은 아이콘 (보통 hIcon과 같은걸 사용)
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

	ShowWindow(hWnd, nCMdShow);	//	윈도우 띄우기
	UpdateWindow(hWnd);		// 윈도우 업데이트

	while (GetMessage(&Message, 0, 0, 0)) {		//	메시지 루프 (메시지 받아오는 역할) -  종료메시지가 들어올때까지 반복
		TranslateMessage(&Message);		//	메시지 번역
		DispatchMessage(&Message);	//	메시지 처리
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
		} while (n == 0 || n == 1 || m == 0 || m == 1 ||k == 0 || k == 1);
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