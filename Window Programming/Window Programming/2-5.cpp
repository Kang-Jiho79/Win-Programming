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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// 메인함수 
{
	srand(time(NULL));
	for (int i = 0; i < 10; i++) {
		r[i] = rand() % 255;
		g[i] = rand() % 255;
		b[i] = rand() % 255;
	}
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