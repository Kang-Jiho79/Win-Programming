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

// 200 x 150 사각형그리기
void quadrangle(HDC hdc, int x, int y)
{
	Rectangle(hdc, x - 100, y - 75, x + 100, y + 75);
}

// 100 x 100 원 그리기
void circle(HDC hdc, int x, int y)
{
	Ellipse(hdc, x - 50, y - 50, x + 50, y + 50);
}
// 200 x 100 타원 그리기
void oval(HDC hdc, int x, int y)
{
	Ellipse(hdc, x - 75, y - 50, x + 75, y + 50);
}

// 100 x 100 모레시계 그리기
void sandglass(HDC hdc, int x, int y)
{
	POINT point[7] = { {x,y},{x - 50,y - 50},{x + 50,y - 50},{x,y},{x + 50,y + 50},{x - 50,y + 50},{x,y} };
	Polygon(hdc, point, 7);
}
// 100 x 100 누워있는 모레시계그리기
void liedownsandglass(HDC hdc, int x, int y)
{
	POINT point[7] = { {x,y},{x - 50,y - 50},{x - 50,y + 50},{x,y},{x + 50,y - 50},{x + 50,y + 50},{x,y} };
	Polygon(hdc, point, 7);
}

// 100 x 100 오각형 그리기
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
// 100 x 100 거꾸로된 오각형 그리기
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

// 100 x 100 파이 그리기
void pie(HDC hdc, int x, int y)
{
	Pie(hdc, x - 50, y - 50, x + 50, y + 50, x, y - 50, x + 50, y);
}
// 100 x 100 나머지 파이부분 그리기
void remainpie(HDC hdc, int x, int y)
{
	Pie(hdc, x - 50, y - 50, x + 50, y + 50, x + 50, y, x, y - 50);
}

int present = 4; // 현재 중앙의 그려져있는 도형 0 : 원/ 1 : 모레시계/ 2 : 오각형/ 3 : 파이
bool pushcheck[5] = { false }; // 눌려있는지 확인
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
