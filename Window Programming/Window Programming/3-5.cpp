//실습 3-5번
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define size 40

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

struct Car
{
	int		x, y;
	int		dx, dy;
	bool	pair;	// copy활성화
};
struct Person
{
	int		x, y;
	int		dx, dy;
	bool	edge;	// 끝에 있는지
};
Car		original[4];
Car		copy[4];
Person person;
int		Time = 0;					// 신호등 신호 시간
bool	Timecheck = false;			// 신호등 타이머 사용중 확인
bool	widthcheck = true;			// 가로가 파란불인지
bool	widthcarcheck = false;		// 가로로 지나가는 차가 없는지
bool	lengthcarcheck = false;		// 세로로 지나가는 차가 없는지
bool	humancheck = false;			// 지나가는 사람이 없는지
bool	stophuman = false;			// 사람 지나가는거 통제
bool	crosscheck = false;			// 대각선으로 지나가는지
int		speed = 5;					// 자동차 속도

void gamesetting(RECT rt)
{
	for (int i = 0; i < 4; i++) {
		if (i > 1) {
			original[i].x = (i == 2 ? rt.right * 5 / 12 : rt.right * 7 / 12);
			original[i].y = (i == 2 ? size * 9 / 4 : rt.bottom - size * 9 / 4);
			original[i].dx = 0;
			original[i].dy = (i == 2 ? 1 : -1);
		}
		else {
			original[i].x = (i == 0 ? size * 9 / 4 : rt.right - size * 9 / 4);
			original[i].y = (i == 0 ? rt.bottom * 7 / 12 : rt.bottom * 5 / 12);
			original[i].dx = (i == 0 ? 1 : -1);
			original[i].dy = 0;
		}
		original[i].pair = true;
		copy[i] = original[i];
		copy[i].pair = false;
	}
	person.x = rt.right / 3 - size / 2;
	person.y = rt.bottom * 2 / 3 + size / 2;
	person.dx = 0;
	person.dy = -1;
	person.edge = true;
}

void drawroad(HDC hdc,RECT rt)
{
	int x = 0, y = 0;
	HPEN pen = CreatePen(PS_SOLID, 2, BLACK_PEN);
	HPEN oldpen = (HPEN)SelectObject(hdc, pen);
	for (int i = 0; i < 4; i++)
	{
		MoveToEx(hdc, x, 0, NULL);
		LineTo(hdc, x, rt.bottom);
		MoveToEx(hdc, 0, y, NULL);
		LineTo(hdc, rt.right, y);
		x += rt.right / 3;
		y += rt.bottom / 3;
	}
	Rectangle(hdc, rt.right / 3 - size, 0, rt.right / 3, size);
	Rectangle(hdc, 0, rt.bottom * 2 / 3, size, rt.bottom * 2 / 3 + size);
	pen = CreatePen(PS_DOT, 1, BLACK_PEN);
	oldpen = (HPEN)SelectObject(hdc, pen);
	MoveToEx(hdc, rt.right / 2, 0, NULL);
	LineTo(hdc, rt.right / 2, rt.bottom);
	MoveToEx(hdc, 0, rt.bottom / 2, NULL);
	LineTo(hdc, rt.right, rt.bottom / 2);
	SelectObject(hdc, oldpen);
	DeleteObject(pen);
}

void square(HDC hdc, int x1, int y1, int x2, int y2, Car c)
{
	if (c.dx == 1 || c.dx == -1) {
		Rectangle(hdc, x1 - size, y1 - size / 2, x1 + size, y1 + size / 2);
		Rectangle(hdc, x2 - size, y2 - size / 2, x2 + size, y2 + size / 2);
	}
	else if (c.dy == 1 || c.dy == -1) {
		Rectangle(hdc, x1 - size / 2, y1 - size, x1 + size / 2, y1 + size);
		Rectangle(hdc, x2 - size / 2, y2 - size, x2 + size / 2, y2 + size);
	}
}

void drawcar(HDC hdc, Car c)
{
	if (!c.pair)
		return;
	HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	if (c.dx == 1 || c.dx == -1)
		square(hdc, c.x - size - size / 4, c.y, c.x + size + size / 4, c.y, c);
	else
		square(hdc, c.x, c.y - size - size / 4, c.x, c.y + size + size / 4, c);
	SelectObject(hdc, oldbrush);
	DeleteObject(brush);
}


void movecar(Car& c, RECT rt)
{
	if (c.dx == 1) {
		if (c.x == rt.right / 3 - size * 4) {
			if (widthcheck && !lengthcarcheck && !humancheck && !crosscheck) {
				widthcarcheck = true;
			}
			else
				return;
		}
		c.x += c.dx;
		if (c.x == rt.right * 2 / 3 + size * 9 / 4)
			widthcarcheck = false;
	}
	else if (c.dx == -1) {
		if (c.x == rt.right * 2 / 3 + size * 4) {
			if (widthcheck && !lengthcarcheck && !humancheck && !crosscheck) {
				widthcarcheck = true;
			}
			else
				return;
		}
		c.x += c.dx;
		if (c.x == rt.right / 3 - size * 9 / 4)
			widthcarcheck = false;
	}
	else if (c.dy == 1) {
		if (c.y == rt.bottom / 3 - size * 9 / 4) {
			if (!widthcheck && !widthcarcheck && !humancheck && !crosscheck) {
				lengthcarcheck = true;
			}
			else
				return;
		}
		c.y += c.dy;
		if (c.y == rt.bottom * 2 / 3 + size * 9 / 4)
			lengthcarcheck = false;
	}
	else if (c.dy == -1) {
		if (c.y == rt.bottom * 2 / 3 + size * 9 / 4) {
			if (!widthcheck && !widthcarcheck && !humancheck && !crosscheck) {
				lengthcarcheck = true;
			}
			else
				return;
		}
		c.y += c.dy;
		if (c.y == rt.bottom / 3 - size * 9 / 4)
			lengthcarcheck = false;
	}
}

void Circle(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
	HBRUSH brush = CreateSolidBrush(color);
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	Ellipse(hdc, x1, y1, x2, y2);
	SelectObject(hdc, oldbrush);
	DeleteObject(brush);
}

void movecopy(Car& original, Car& copy, RECT rt) {
	if (original.dx == 1) {
		if (original.x - size * 9 / 4 > rt.right) {
			original = copy;
			copy.pair = false;
		}
		else if (original.x + size * 9 / 4 > rt.right) {
			copy.pair = true;
			copy.x = original.x - rt.right;
		}
	}
	else if (original.dx == -1) {
		if (original.x + size * 9 / 4 < rt.left) {
			original = copy;
			copy.pair = false;
		}
		else if (original.x - size * 9 / 4 < rt.left) {
			copy.pair = true;
			copy.x = original.x + rt.right;
		}
	}
	else if (original.dy == 1) {
		if (original.y - size * 9 / 4 > rt.bottom) {
			original = copy;
			copy.pair = false;
		}
		else if (original.y + size * 9 / 4 > rt.bottom) {
			copy.pair = true;
			copy.y = original.y - rt.bottom;
		}
	}
	else if (original.dy == -1) {
		if (original.y + size * 9 / 4 < rt.top) {
			original = copy;
			copy.pair = false;
		}
		else if (original.y - size * 9 / 4 < rt.top) {
			copy.pair = true;
			copy.y = original.y + rt.bottom;
		}
	}
}

void moveperson(RECT rt)
{
	if (stophuman || crosscheck || humancheck)
		return;
	if (person.edge) {
		if (widthcheck || widthcarcheck)
			return;
		person.y += person.dy;
		person.edge = false;
	}
	else {
		if (person.y == rt.bottom / 3 - size / 2) {
			person.edge = true;
			person.dy = 1;
			stophuman = true;
			lengthcarcheck = false;
			return;
		}
		else if (person.y == rt.bottom * 2 / 3 + size / 2) {
			person.edge = true;
			person.dy = -1;
			stophuman = true;
			lengthcarcheck = false;
			return;
		}
		person.y += person.dy;
		lengthcarcheck = true;
	}
}

void cross(RECT rt) 
{
	if (widthcarcheck || lengthcarcheck) return;
	if (!crosscheck && !humancheck) return;
	if (person.edge) {
		person.x += person.dx;
		person.y += person.dy;
		person.edge = false;
		humancheck = true;
	}
	if (person.x == rt.right * 2 / 3 + size / 2 || person.x == rt.right / 3 - size / 2) {
		person.edge = true;
		person.dx *= -1;
		person.dy *= -1;
		humancheck = false;
		return;
	}
	person.x += person.dx;
	person.y += person.dy;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, mDC;
	HBITMAP hBitmap;
	RECT rt;
	GetClientRect(hWnd, &rt);
	switch (iMessage) {
	case WM_CREATE:
		gamesetting(rt);
		SetTimer(hWnd, 1, 15, NULL);
		SetTimer(hWnd, 2, 1000, NULL);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		drawroad(mDC, rt);
		if (widthcheck) {
			Circle(mDC, 0, rt.bottom * 2 / 3, size, rt.bottom * 2 / 3 + size, RGB(0, 255, 0));
			Circle(mDC, rt.right / 3 - size, 0, rt.right / 3, size, RGB(255, 0, 0));
		}
		else {
			Circle(mDC, 0, rt.bottom * 2 / 3, size, rt.bottom * 2 / 3 + size, RGB(255, 0, 0));
			Circle(mDC, rt.right / 3 - size, 0, rt.right / 3, size, RGB(0, 255, 0));
		}
		if (Time == 5) {
			Circle(mDC, 0, rt.bottom * 2 / 3, size, rt.bottom * 2 / 3 + size, RGB(255, 255, 0));
		}
		else if (Time == 10) {
			Circle(mDC, rt.right / 3 - size, 0, rt.right / 3, size, RGB(255, 255, 0));
		}
		if (crosscheck) {
			Circle(mDC, 0, rt.bottom * 2 / 3, size, rt.bottom * 2 / 3 + size, RGB(255, 0, 0));
			Circle(mDC, rt.right / 3 - size, 0, rt.right / 3, size, RGB(255, 0, 0));
		}
		Ellipse(mDC, person.x - size / 2, person.y - size / 2, person.x + size / 2, person.y + size / 2);
		for (int i = 0; i < 4; ++i) {
			drawcar(mDC, original[i]);
			drawcar(mDC, copy[i]);
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if ((x >= 0 && y >= rt.bottom * 2 / 3 && x <= size && y <= rt.bottom * 2 / 3 + size) || (x >= rt.right / 3 - size && y >= 0 && x <= rt.right / 3 && y <= size)) {
			if (!Timecheck) {
				widthcheck = !widthcheck;
			}
		}
		else {
			if (person.y == rt.bottom / 3 - size / 2 || person.y == rt.bottom * 2 / 3 + size / 2) {
				crosscheck = true;
				if (person.x == rt.right / 3 - size / 2)
					person.dx = 1;
				else
					person.dx = -1;
			}
		}
	}
	break;
	case WM_RBUTTONDOWN:{
		if (crosscheck) {
			crosscheck = false;
			widthcheck = true;
			Time = 0;
		}
	}
	break;
	case WM_CHAR:
		if (wParam == 'Q' || wParam == 'q')
			PostQuitMessage(0);
		else if (wParam == 'A' || wParam == 'a') {
			Timecheck = !Timecheck;
			Time = 0;
			widthcheck = true;
		}
		else if (wParam == '+')
			speed++;
		else if (wParam == '-')
			speed--;
		break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case 1:
			for (int j = 0; j < speed; j++) {
				for (int i = 0; i < 4; i++) {
					movecar(original[i], rt);
					movecopy(original[i], copy[i], rt);
				}
			}
			moveperson(rt);
			cross(rt);
			break;
		case 2: {
			if (Timecheck) {
				if (Time == 10) {
					widthcheck = !widthcheck;
					Time = 0;
					stophuman = false;
				}
				else if (Time == 5) {
					widthcheck = !widthcheck;
					Time++;
				}
				else
					Time++;
			}
		}
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}