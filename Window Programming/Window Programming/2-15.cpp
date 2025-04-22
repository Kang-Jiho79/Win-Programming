#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

#define max_x 800
#define max_y 600
#define shapecount 5

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

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1100, NULL, (HMENU)NULL, hInstance, NULL);	//윈도우 만들기 함수

	ShowWindow(hWnd, nCMdShow);	//	윈도우 띄우기
	UpdateWindow(hWnd);		// 윈도우 업데이트

	while (GetMessage(&Message, 0, 0, 0)) {		//	메시지 루프 (메시지 받아오는 역할) -  종료메시지가 들어올때까지 반복
		TranslateMessage(&Message);		//	메시지 번역
		DispatchMessage(&Message);	//	메시지 처리
	}
	return Message.wParam;
}

struct Shape
{
	int x, y;
	int type;
	int thickness;
	int size;
	COLORREF inner, line;
};

Shape shape[5];
WCHAR errormessage[20] = L"에러 발생!";
bool errorcheck = false;
wstring status[shapecount];
WCHAR Input[30];
WCHAR Caretpoint[30];
int inputcount = 0;
int now = 0;
int prior = 0;
int Next = 0;
bool allcheck = false;
wstring Inputresult[6];

void Inputcheck()
{
	wstringstream ss(Input);
	wstring w;
	int i = 0;
	while (ss >> w) {
		wstring word = w;
		Inputresult[i++] = w;
	}
	if (i != 6) {
		for (int i = 0; i < 30; i++) {
			Input[i] = '\0';
			Caretpoint[i] = '\0';
		}
		inputcount = 0;
		errorcheck = true;
		return;
	}
	if (stoi(Inputresult[0]) == 2 || stoi(Inputresult[0]) == 3 || stoi(Inputresult[0]) == 4 || stoi(Inputresult[0]) == 5 || stoi(Inputresult[0]) == 6) {
		if (stoi(Inputresult[1]) >= 0 && stoi(Inputresult[2]) >= 0 && stoi(Inputresult[3]) >= 0 && stoi(Inputresult[4]) >= 0 && stoi(Inputresult[1]) <= max_x && stoi(Inputresult[3]) <= max_x && stoi(Inputresult[2]) <= max_y && stoi(Inputresult[4]) <= max_y) {
			if (stoi(Inputresult[5]) >= 1 && stoi(Inputresult[5]) <= 10) {
				for (int i = 0; i < shapecount; i++) {
					if (shape[i].type == 0) {
						int x1 = stoi(Inputresult[1]), x2 = stoi(Inputresult[3]);
						int y1 = stoi(Inputresult[2]), y2 = stoi(Inputresult[4]);
						shape[i].type = stoi(Inputresult[0]);
						shape[i].x = rand() % (x1 > x2 ? (x1 - x2 + 1) + x2 : (x2 - x1 + 1) + x1);
						shape[i].y = rand() % (y1 > y2 ? (y1 - y2 + 1) + y2 : (y2 - y1 + 1) + y1);
						shape[i].thickness = stoi(Inputresult[5]);
						status[i] += L"shape : " + to_wstring(i);
						status[i] += L" X : " + to_wstring(shape[i].x);
						status[i] += L" Y : " + to_wstring(shape[i].y);
						status[i] += L" WIDTH : " + to_wstring(shape[i].thickness);
						prior = now;
						now = i;
						for (int i = 0; i < 30; i++) {
							Input[i] = '\0';
							Caretpoint[i] = '\0';
						}
						inputcount = 0;
						return;
					}
				}

			}
		}
	}
	for (int i = 0; i < 30; i++) {
		Input[i] = '\0';
		Caretpoint[i] = '\0';
	}
	inputcount = 0;
	errorcheck = true;
	return;
}

void setstatus()
{
	status[now].clear();
	status[now] += L"shape : " + to_wstring(now);
	status[now] += L" X : " + to_wstring(shape[now].x);
	status[now] += L" Y : " + to_wstring(shape[now].y);
	status[now] += L" WIDTH : " + to_wstring(shape[now].thickness);
}

// 게임 기초 세팅
void settinggame()
{
	srand((unsigned)time(0));
	for (int i = 0; i < shapecount; i++) {
		shape[i].type = 0;
		shape[i].inner = RGB(rand() % 256, rand() % 256, rand() % 256);
		shape[i].line = RGB(rand() % 256, rand() % 256, rand() % 256);
		shape[i].size = 30;
		status[i].clear();
	}
}

void MovePlayer(int dx, int dy) {
	if (shape[now].type == 0)
		return;
	int nx = shape[now].x + dx;
	int ny = shape[now].y + dy;
	if (nx < 0 || ny < 0 || nx >= max_x || ny >= max_y)
		return;

	shape[now].x = nx;
	shape[now].y = ny;
}

void drawline(HDC hdc, int x, int y, int size)
{
	MoveToEx(hdc, x - size, y - size, NULL);
	LineTo(hdc, x + size, y + size);
}
// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}
// 삼각형 그리기
void triangle(HDC hdc, int x, int y, int size)
{
	POINT point[3] = { {x,y - size * 2 / 3}, {x - size / 2, y + size / 3},{x + size / 2, y + size / 3} };
	Polygon(hdc, point, 3);
}

// 오각형 그리기
void pentagon(HDC hdc, int x, int y, int size)
{
	double pi = 3.14;
	int r = size * 3 / 5;
	POINT point[5];
	for (int i = 0; i < 5; i++) {
		double angle = -pi / 2 + i * (2 * pi / 5);
		point[i].x = (x + r * cos(angle));
		point[i].y = (y + r * sin(angle));
	}
	Polygon(hdc, point, 5);
}

// 육각형 그리기
void hexagon(HDC hdc, int x, int y, int size)
{
	POINT point[6] = { {x - size / 2,y - size / 4},{x,y - size / 2}, {x + size / 2,y - size / 4}, {x + size / 2,y + size / 4},{x,y + size / 2},{x - size / 2,y + size / 4} };
	Polygon(hdc, point, 6);
}

void drawshape(HDC hdc, Shape shape)
{
	if (shape.type == 0)
		return;
	setstatus();
	switch (shape.type)
	{
	case 2: {
		HPEN pen = CreatePen(PS_SOLID, shape.thickness, shape.line);
		SelectObject(hdc, pen);
		drawline(hdc, shape.x, shape.y, shape.size);
		DeleteObject(pen);
	}
		  break;
	case 3: {
		HBRUSH brush = CreateSolidBrush(shape.inner);
		HPEN pen = CreatePen(PS_SOLID, shape.thickness, shape.line);
		SelectObject(hdc, brush);
		SelectObject(hdc, pen);
		triangle(hdc, shape.x, shape.y, shape.size);
		DeleteObject(brush);
		DeleteObject(pen);
	}
		  break;
	case 4: {
		HBRUSH brush = CreateSolidBrush(shape.inner);
		HPEN pen = CreatePen(PS_SOLID, shape.thickness, shape.line);
		SelectObject(hdc, brush);
		SelectObject(hdc, pen);
		square(hdc, shape.x, shape.y, shape.size);
		DeleteObject(brush);
		DeleteObject(pen);
	}
		  break;

	case 5: {
		HBRUSH brush = CreateSolidBrush(shape.inner);
		HPEN pen = CreatePen(PS_SOLID, shape.thickness, shape.line);
		SelectObject(hdc, brush);
		SelectObject(hdc, pen);
		pentagon(hdc, shape.x, shape.y, shape.size);
		DeleteObject(brush);
		DeleteObject(pen);
	}
		  break;
	case 6: {
		HBRUSH brush = CreateSolidBrush(shape.inner);
		HPEN pen = CreatePen(PS_SOLID, shape.thickness, shape.line);
		SelectObject(hdc, brush);
		SelectObject(hdc, pen);
		hexagon(hdc, shape.x, shape.y, shape.size);
		DeleteObject(brush);
		DeleteObject(pen);
	}
		  break;
	default:
		break;
	}

}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	SIZE size;
	HDC hDC;
	HPEN hDefPen;
	HBRUSH hDefBrush, hRedBrush, hBlueBrush, hGreenBrush, hYellowBrush, hTurquoiseBrush;
	RECT Now;
	switch (iMessage) {
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		settinggame();
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		Rectangle(hdc, max_x / 2 - 55, 0, max_x / 2 + 200, 30);
		TextOut(hdc, max_x / 2 - 50, 5, Input, _tcslen(Input));
		GetTextExtentPoint32(hdc, Caretpoint, inputcount, &size);
		SetCaretPos(size.cx + max_x / 2 - 50, 5);
		if (errorcheck) {
			errorcheck = false;
			TextOut(hdc, 0, 0, errormessage, _tcslen(errormessage));
		}
		for (int i = 0; i < shapecount; i++) {
			if (shape[i].type != 0) {
				TextOut(hdc, 800, (i + 1) * 20, status[i].c_str(), status[i].length());
				if (i == now)
					TextOut(hdc, 780, (i + 1) * 20, L"->", strlen("->"));
			}
		}
		if (allcheck) {
			allcheck = false;
			for (int i = 0; i < shapecount; i++)
				drawshape(hdc, shape[i]);
		}
		else
			drawshape(hdc, shape[now]);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (wParam == 'Q')
			PostQuitMessage(0);
		if (shape[now].type != 0) {
			if (wParam == VK_UP) MovePlayer(0, -10);
			else if (wParam == VK_LEFT) MovePlayer(-10, 0);
			else if (wParam == VK_DOWN) MovePlayer(0, 10);
			else if (wParam == VK_RIGHT) MovePlayer(10, 0);
			else if (wParam == 'D') settinggame();
			else if (wParam == 'L')shape[now].line = RGB(rand() % 256, rand() % 256, rand() % 256);
			else if (wParam == 'I')shape[now].inner = RGB(rand() % 256, rand() % 256, rand() % 256);
			else if (wParam == 'P') {
				if (now != prior)
					Next = now, now = prior;
			}
			else if (wParam == 'N') {
				if (now != Next)
					prior = now, now = Next;
			}
			else if (wParam == 'A') allcheck = true;
		}
		else {
			if (wParam == VK_UP) errorcheck = true;
			else if (wParam == VK_LEFT) errorcheck = true;
			else if (wParam == VK_DOWN) errorcheck = true;
			else if (wParam == VK_RIGHT) errorcheck = true;
			else if (wParam == 'D') errorcheck = true;
			else if (wParam == 'L') errorcheck = true;
			else if (wParam == 'I') errorcheck = true;
			else if (wParam == 'P') errorcheck = true;
			else if (wParam == 'N') errorcheck = true;
			else if (wParam == 'A') errorcheck = true;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		if (wParam == '+') {
			if (shape[now].type != 0) {
				if (shape[now].thickness >= 10) {
					if (shape[now].size < 200)
						shape[now].size += 5;
				}
				else
					shape[now].thickness++;
			}
			else errorcheck = true;
		}
		else if (wParam == '-') {
			if (shape[now].type != 0) {
				if (shape[now].thickness <= 1) {
					if (shape[now].size > 10)
						shape[now].size -= 5;
				}
				else
					shape[now].thickness--;
			}
			else errorcheck = true;
		}
		else if ((wParam >= 0x30 && wParam <= 0x39) || wParam == 0x20) {
			Input[inputcount] = wParam;
			Caretpoint[inputcount++] = wParam;
		}
		else if (wParam == VK_RETURN)
			Inputcheck();
		else if (wParam == VK_BACK) {
			if (inputcount > 0)
				Input[(inputcount--) - 1] = '\0';
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}
