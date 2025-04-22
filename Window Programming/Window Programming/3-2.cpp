#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define tablecount 25
#define cellsize 20
#define tailscount 20

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

struct Cell
{
	int x, y;			// 위치
	int size;			// 크기
	COLORREF color;		// 색
};

struct Circle
{
	int x, y;			// 위치
	int px, py;			// 이전 위치
	int dx, dy;			// 이동 방향
	COLORREF color;		// 색
	int movetype;		// 이동타입
	int prevtail;		// 앞에 붙어있는 꼬리의 번호
	int endtail;		// 맨뒤 꼬리의 번호
	int number;			// 꼬리에서 몇번째인지
};

Circle player;
Cell cell[tailscount];
Circle tails[tailscount];
int linklist[tailscount];
int linkcount = 0;

// 게임 초기세팅
void settinggame()
{
	srand(unsigned(time(NULL)));
	player.x = 0;
	player.y = 0;
	player.dx = 1;
	player.dy = 0;
	player.number = 0;
	player.color = RGB(rand() % 256, rand() % 256, rand() % 256);
	player.movetype = 0;
	player.endtail = -1;
	for (int i = 0; i < tailscount; i++)
	{
		tails[i].x = -1;
		tails[i].y = -1;
		tails[i].number = -1;
		tails[i].prevtail = -1;
		tails[i].endtail = i;
		cell[i].x = rand() % tablecount;
		cell[i].y = rand() % tablecount;
		cell[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
	}
}

// 판 그리기
void drawtable(HDC hdc)
{
	for (int i = 0; i <= tablecount; i++) {
		MoveToEx(hdc, i * cellsize, 0, NULL);
		LineTo(hdc, i * cellsize, cellsize * tablecount);
		MoveToEx(hdc, 0, i * cellsize, NULL);
		LineTo(hdc, cellsize * tablecount, i * cellsize);
	}
}


// 사각형 그리기
void square(HDC hdc, int x, int y, int size)
{
	Rectangle(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}
// 원 그리기
void circle(HDC hdc, int x, int y, int size)
{
	Ellipse(hdc, x - size / 2, y - size / 2, x + size / 2, y + size / 2);
}

void tailset(Cell cell, Circle tail)
{
	tail.x = cell.x;
	tail.y = cell.y;
	tail.number = 0;
	tail.movetype = rand() % 5 + 1;
	tail.color = cell.color;
	switch (tail.movetype)
	{
	case 1: {
		tail.dx = 1;
		tail.dy = 0;
	}
		break;
	case 2: {
		tail.dx = 0;
		tail.dy = 1;
	}
		break;
	case 3: {
		tail.dx = 1;
		tail.dy = 1;
	}
		break;
	case 4: {
		tail.dx = 0;
		tail.dy = 0;
	}
		break;
	case 5: {
		tail.dx = 0;
		tail.dy = 0;
	}
		  break;
	default:
		break;
	}
	cell.x = -1;
	cell.y = -1;
}

void drawtail(HDC hdc, Circle c) {
	if (c.x == -1)
		return;
	HBRUSH brush = CreateSolidBrush(c.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	circle(hdc, c.x*cellsize + cellsize/2, c.y * cellsize + cellsize / 2, cellsize - (c.number + 1));
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

void drawcell(HDC hdc, Cell c) {
	if (c.x == -1)
		return;
	HBRUSH brush = CreateSolidBrush(c.color);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
	square(hdc, c.x * cellsize + cellsize / 2, c.y * cellsize + cellsize / 2, cellsize/2);
	SelectObject(hdc, oldBrush);
	DeleteObject(brush);
}

// 꼬리 움직임
void movetail()
{
	for (int i = 0; i < tailscount; i++) {
		if (tails[i].number == -1)
			return;
		tails[i].px = tails[i].x;
		tails[i].py = tails[i].y;
	}
	for (int i = 0; i < tailscount; i++) {
		if (tails[i].number == -1)
			return;
		if (tails[i].prevtail != -1) {
			if (tails[i].prevtail == 20) {
				tails[i].x = player.x;
				tails[i].y = player.y;
			}
			else {
				tails[i].x = tails[tails[i].prevtail].px;
				tails[i].y = tails[tails[i].prevtail].py;
			}
		}
		else {
			tails[i].x += tails[i].dx;
			tails[i].y += tails[i].dy;
		}
	}
}
// 플레이어 움직임
void moveplayer()
{
	int nx = player.x + player.dx;
	int ny = player.y + player.dy;
	if (player.dx != 0) {
		if (player.x == tablecount - 1 && player.dx == 1) {
			if (player.y == tablecount - 1) {
				player.y -= 1;
				player.dx = -1;
				player.dy = 0;
			}
			else {
				player.y += 1;
				player.dx = -1;
				player.dy = 0;
			}
			nx = player.x + player.dx + 1;
			ny = player.y + player.dy;
		}
		else if (player.x == 0 && player.dx == -1) {
			if (player.y == tablecount - 1) {
				player.y -= 1;
				player.dx = 1;
				player.dy = 0;
			}
			else {
				player.y += 1;
				player.dx = 1;
				player.dy = 0;
			}
			nx = player.x + player.dx-1;
			ny = player.y + player.dy;
		}
	}
	else {
		if (player.y == tablecount - 1 && player.dy == 1) {
			if (player.x == tablecount - 1) {
				player.x -= 1;
				player.dx = 0;
				player.dy = -1;
			}
			else {
				player.x += 1;
				player.dx = 0;
				player.dy = -1;
			}
			nx = player.x + player.dx;
			ny = player.y + player.dy+1;
		}
		else if (player.y == 0 && player.dy == -1) {
			if (player.x == tablecount - 1) {
				player.x -= 1;
				player.dx = 0;
				player.dy = 1;
			}
			else {
				player.x += 1;
				player.dx = 0;
				player.dy = 1;
			}
			nx = player.x + player.dx;
			ny = player.y + player.dy-1;
		}
	}

	movetail();
	player.x = nx;
	player.y = ny;
	for (int i = 0; i < tailscount; i++) {
		if (player.x == cell[i].x && player.y == cell[i].y) {
			tailset(cell[i], tails[i]);
			break;
		}
		else if(tails[i].number == -1){
			if (player.x == tails[i].x && player.y == tails[i].y) {
				if (player.endtail == -1) {
					tails[i].number = 0;
					tails[i].prevtail = 20;
					player.endtail = tails[i].endtail;
				}
				else {
					tails[i].number = tails[player.endtail].number + 1;
					tails[i].prevtail = player.endtail;
					player.endtail = tails[i].endtail;
				}
				if (tails[i].endtail != i) {
					int temp = tails[i].endtail;
					while (temp != i) {
						tails[temp].number += tails[i].number + 1;
						temp = tails[temp].prevtail;
					}
				}
				break;
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, mDC;
	HBITMAP hBitmap;
	RECT rt;
	switch (iMessage) {
	case WM_CREATE:
		settinggame();
		SetTimer(hWnd, 1, 100, NULL);
		break;
	case WM_PAINT:
	{
		GetClientRect(hWnd, &rt);
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		drawtable(mDC);
		for (int i = 0; i < tailscount; i++) {
			drawcell(mDC, cell[i]);
			drawtail(mDC, tails[i]);
		}
		drawtail(mDC, player);
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (wParam == 'Q')
			PostQuitMessage(0);
		else if (wParam == VK_UP) { player.dx = 0; player.dy = -1; }
		else if (wParam == VK_LEFT) { player.dx = -1; player.dy = 0; }
		else if (wParam == VK_RIGHT) { player.dx = 1; player.dy = 0; }
		else if (wParam == VK_DOWN) { player.dx = 0; player.dy = 1; }
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER: {
		moveplayer();
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