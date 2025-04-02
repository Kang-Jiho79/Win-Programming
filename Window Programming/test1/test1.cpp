#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>

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
	HPEN hDefPen, hSelectPen;
	HBRUSH hDefBrush, hSelectBrush;
	RECT Now;
	POINT Triangle[3] = {};
	GetClientRect(hWnd, &Now);
	int check = 1;
	int x = Now.right / 2, y = Now.bottom / 2;
	int Nowx, Nowy;
	switch (iMessage) {
	
	case WM_CHAR:
	{
		
	
	}
	break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		Rectangle(hDC, x, y, x + 300, y + 100);
		if (check == 1) {
			hSelectPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			hSelectBrush = CreateSolidBrush(RGB(255, 0, 0));
			hDefPen = (HPEN)SelectObject(hDC, hSelectPen);
			hDefBrush = (HBRUSH)SelectObject(hDC, hSelectBrush);
			for (int i = 0; i < 10; i++) {
				Nowx = x + (25 * (i + 1));
				Nowy = y;
				Triangle[0] = { Nowx, (Nowy + 6) };
				Triangle[1] = { Nowx + 10,Nowy - 3 };
				Triangle[2] = { Nowx - 10,Nowy - 3 };
				Polygon(hDC, Triangle, 3);
			}
			SelectObject(hDC, hDefPen);
			SelectObject(hDC, hDefBrush);
			DeleteObject(hSelectPen);
			DeleteObject(hSelectBrush);
		}

		else if (check == 2) {

		}
		else if (check == 3) {

		}
		EndPaint(hWnd, &ps);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
