#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "resource1.h"

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlalog_Proc(HWND, UINT, WPARAM, LPARAM);

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

POINT		point = {100,100};
POINT		d = { 0,0 };
POINT		p = { 100,100 };
int			size = 10;
bool		rect = true;
COLORREF	color = RGB(0, 0, 0);
POINT		ptLT = {0,0};
bool		drawcheck = false;
bool		movecheck = false;
bool		back = false;

void draw(HDC hdc)
{
	HBRUSH brush = CreateSolidBrush((color == RGB(0, 0, 0) ? RGB(255, 255, 255) : color));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, brush);
	if (rect) {
		Rectangle(hdc, point.x - size, point.y - size, point.x + size, point.y + size);
	}
	else {
		Ellipse(hdc, point.x - size, point.y - size, point.x + size, point.y + size);
	}
	(HBRUSH)SelectObject(hdc, oldbrush);
	DeleteObject(brush);
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
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);

		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dlalog_Proc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK Dlalog_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, mDC;
	HBITMAP hBitmap;
	RECT rt;
	GetClientRect(hDlg, &rt);
	switch (iMsg) {
	case WM_INITDIALOG:
		SetTimer(hDlg, 1, 10, NULL);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hDlg, &ps);
		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		Rectangle(mDC, 0, 0, rt.right, rt.bottom);
		draw(mDC);
		if (ptLT.x != 0 && ptLT.y != 0) {
			MoveToEx(mDC, p.x, p.y, NULL);
			LineTo(mDC, ptLT.x, ptLT.y);
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hDlg, &ps);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RECTANGLE:
			rect = true;
			break;
		case IDC_CIRCLE:
			rect = false;
			break;
		case IDC_SMALL:
			size = 10;
			break;
		case IDC_MEDIUM:
			size = 15;
			break;
		case IDC_LARGE:
			size = 20;
			break;
		case IDC_RED:
			color += RGB(255, 0, 0);
			break;
		case IDC_BLUE:
			color += RGB(0, 0, 255);
			break;
		case IDC_DRAW:
			drawcheck = true;
			break;
		case IDC_MOVE:
			movecheck = true;
			break;
		case IDC_QUIT:
			EndDialog(hDlg,NULL);
			break;
		}
		InvalidateRect(hDlg, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
	{
		if (drawcheck) {
			ptLT.x = LOWORD(lParam);
			ptLT.y = HIWORD(lParam);
		}
		InvalidateRect(hDlg, NULL, FALSE);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (drawcheck) {
			ptLT.x = LOWORD(lParam);
			ptLT.y = HIWORD(lParam);
			drawcheck = false;
			if (ptLT.x - point.x > 0)
				d.x = 1;
			else if (ptLT.x - point.x < 0)
				d.x = -1;
			if (ptLT.y - point.y > 0)
				d.y = 1;
			else if (ptLT.y - point.y < 0)
				d.y = -1;
		}
		InvalidateRect(hDlg, NULL, FALSE);
	}
	break;
	case WM_TIMER:
	{
		if (movecheck) {
			if (back) {
				if (p.x - point.x > 0)
					d.x = (p.x - point.x) / 100;
				else if (p.x - point.x < 0)
					d.x = (p.x - point.x) / 100;
				else
					d.x = 0;
				if (p.y - point.y > 0)
					d.y = (p.y - point.y)/100;
				else if (p.y - point.y < 0)
					d.y = (p.y - point.y) / 100;
				else
					d.y = 0;
			}
			else {
				if (ptLT.x - point.x > 0)
					d.x = (ptLT.x - point.x) /100;
				else if (ptLT.x - point.x < 0)
					d.x = (ptLT.x - point.x) / 100;
				else
					d.x = 0;
				if (ptLT.y - point.y > 0)
					d.y = (ptLT.y - point.y) / 100;
				else if (ptLT.y - point.y < 0)
					d.y = (ptLT.y - point.y) / 100;
				else
					d.y = 0;
			}
			if (point.x == ptLT.x && point.y == ptLT.y) {
				back = true;
			}
			else if (point.x == p.x && point.y == p.y) {
				back = false;
			}
			point.x += d.x;
			point.y += d.y;
			InvalidateRect(hDlg, NULL, FALSE);
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return 0;
}