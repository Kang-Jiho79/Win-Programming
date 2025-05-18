#pragma comment (lib, "msimg32.lib")
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <atlImage.h>
#include <cmath>
#include "resource1.h"

#define line 6
#define startcount 2
#define boardsize 500

#define _LT 1
#define _RT 2
#define _UP 3
#define _DN 4

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"5-7";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevinstance, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

struct NumBlock
{
	POINT			rpt;
	POINT			bpt;
	POINT			dpt;
	int				num;
	int				size;
};

struct Board
{
	POINT pt;
	int width;
	int height;
	int arrtype[line][line];
};

NumBlock numblock[line * line];
Board board;
int arrow;
bool move = false;
bool endcheck = false;
int obs = 2;

void setting(RECT rt)
{
	board.pt = { rt.right / 2 - boardsize / 2 , rt.bottom / 2 - boardsize / 2 };
	board.width = boardsize;
	board.height = boardsize;
	for (int i = 0; i < line; ++i)
		for (int j = 0; j < line; ++j)
			board.arrtype[i][j] = 0;
	for (int i = 0; i < line * line; ++i) {
		numblock[i].rpt = { 0,0 };
		numblock[i].bpt = { -1,-1 };
		numblock[i].dpt = { 0,0 };
		numblock[i].num = 0;
		numblock[i].size = boardsize / line;
	}
	for (int i = 0; i < startcount; ++i) {
		while (true) {
			int x = rand() % line;
			int y = rand() % line;
			if (board.arrtype[y][x] == 0) {
				board.arrtype[y][x] = 1;
				numblock[i].bpt.x = x;
				numblock[i].bpt.y = y;
				numblock[i].num = 2;
				numblock[i].rpt.x = board.pt.x + numblock[i].size * x;
				numblock[i].rpt.y = board.pt.y + numblock[i].size * y;
				break;
			}
		}
	}
	for (int i = 0; i < obs; ++i) {
		while (true) {
			int x = rand() % line;
			int y = rand() % line;
			if (board.arrtype[y][x] == 0) {
				board.arrtype[y][x] = 2;
				break;
			}
		}
	}
}

int nextarrtype(NumBlock nb)
{
	if (nb.num == 0) return 2;
	if (arrow == _LT) {
		if (nb.bpt.x == 0) 
			return 2;
		return board.arrtype[nb.bpt.y][nb.bpt.x - 1];
	}
	else if (arrow == _RT) {
		if (nb.bpt.x == line - 1) 
			return 2;
		return board.arrtype[nb.bpt.y][nb.bpt.x + 1];
	}
	else if (arrow == _UP) {
		if (nb.bpt.y == 0) 
			return 2;
		return board.arrtype[nb.bpt.y - 1][nb.bpt.x];
	}
	else if (arrow == _DN) {
		if (nb.bpt.y == line - 1) 
			return 2;
		return board.arrtype[nb.bpt.y + 1][nb.bpt.x];
	}
}

void startingmovecheck(NumBlock& nb)
{
	if (nb.num == 0 || nb.num == -1) return;
	int temp = nextarrtype(nb);
	board.arrtype[nb.bpt.y][nb.bpt.x] = 1;
	if (temp == 2) 
		return;
	if (arrow == _LT) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x - 1 && numblock[i].bpt.y == nb.bpt.y) {
				if (numblock[i].num != nb.num)
					return;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y][nb.bpt.x--] = 0;
		nb.dpt = { -nb.size,0 };
	}
	else if (arrow == _RT) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x + 1 && numblock[i].bpt.y == nb.bpt.y) {
				if (numblock[i].num != nb.num)
					return;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y][nb.bpt.x++] = 0;
		nb.dpt = { nb.size,0 };
	}
	else if (arrow == _UP) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x && numblock[i].bpt.y == nb.bpt.y - 1) {
				if (numblock[i].num != nb.num)
					return;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y--][nb.bpt.x] = 0;
		nb.dpt = { 0,-nb.size };
	}
	else if (arrow == _DN) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x && numblock[i].bpt.y == nb.bpt.y + 1) {
				if (numblock[i].num != nb.num)
					return;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y++][nb.bpt.x] = 0;
		nb.dpt = { 0,nb.size };
	}
}

void movingblock(NumBlock& nb)
{
	if (nb.num == 0 || (nb.dpt.x == 0 && nb.dpt.y == 0)) return;
	if (nb.dpt.x > 0) {
		nb.dpt.x--;
		nb.rpt.x++;
	}
	else if (nb.dpt.x < 0) {
		nb.dpt.x++;
		nb.rpt.x--;
	}
	else if (nb.dpt.y > 0) {
		nb.dpt.y--;
		nb.rpt.y++;
	}
	else if (nb.dpt.y < 0) {
		nb.dpt.y++;
		nb.rpt.y--;
	}
}

// 숫자 생성
void createblock()
{
	while (true)
	{
		int x = rand() % line;
		int y = rand() % line;
		if (board.arrtype[y][x] == 0) {
			for (int i = 0; i < line * line; ++i) {
				if (numblock[i].num == 0) {
					numblock[i].num = 2;
					numblock[i].bpt.x = x;
					numblock[i].bpt.y = y;
					numblock[i].rpt.x = board.pt.x + numblock[i].size * numblock[i].bpt.x;
					numblock[i].rpt.y = board.pt.y + numblock[i].size * numblock[i].bpt.y;
					board.arrtype[numblock[i].bpt.y][numblock[i].bpt.x] = 1;
					break;
				}
			}
			break;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC, mDC;
	PAINTSTRUCT ps;
	static HBITMAP hBitmap;
	static RECT clRect;
	static CImage img[11];
	static POINT m, oldM;
	static bool drag, start;
	static int target;

	switch (iMsg) {
	case WM_CREATE:
		GetClientRect(hWnd, &clRect);
		img[0].Load(_T("2048\\2048stone_2.png"));
		img[1].Load(_T("2048\\2048stone_4.png"));
		img[2].Load(_T("2048\\2048stone_8.png"));
		img[3].Load(_T("2048\\2048stone_16.png"));
		img[4].Load(_T("2048\\2048stone_32.png"));
		img[5].Load(_T("2048\\2048stone_64.png"));
		img[6].Load(_T("2048\\2048stone_128.png"));
		img[7].Load(_T("2048\\2048stone_256.png"));
		img[8].Load(_T("2048\\2048stone_512.png"));
		img[9].Load(_T("2048\\2048stone_1024.png"));
		img[10].Load(_T("2048\\2048stone_2048.png"));
		srand(time(NULL));
		obs = 2;
		target = 64;
		SetTimer(hWnd, 1, 10, NULL);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, clRect.right, clRect.bottom);

		SelectObject(mDC, (HBITMAP)hBitmap);

		FillRect(mDC, &clRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		for (int i = 0; i < line + 1; ++i) {
			MoveToEx(mDC, board.pt.x, board.pt.y + (board.height / line) * i, NULL);
			LineTo(mDC, board.pt.x + board.width, board.pt.y + (board.height / line) * i);
			MoveToEx(mDC, board.pt.x + (board.width / line) * i, board.pt.y, NULL);
			LineTo(mDC, board.pt.x + (board.width / line) * i, board.pt.y + board.height);
		}

		if (start) {
			for (int i = 0; i < line * line; ++i) {
				if (numblock[i].num > 0)
					img[(int)(log2(numblock[i].num) - 1)].Draw(mDC, numblock[i].rpt.x, numblock[i].rpt.y, numblock[i].size, numblock[i].size, 0, 0, img[0].GetWidth(), img[0].GetHeight());
			}
			for (int i = 0; i <line;++i)
				for (int j = 0; j < line; ++j) {
					if (board.arrtype[i][j] == 2) {
						SelectObject(mDC, (HBRUSH)GetStockObject(BLACK_BRUSH));
						Rectangle(mDC, board.pt.x + board.width / line * j, board.pt.y + board.height / line * i, board.pt.x + board.width / line * j + board.width / line, board.pt.y + board.height / line * i + board.height / line);
					}
				}
		}

		BitBlt(hDC, 0, 0, clRect.right, clRect.bottom, mDC, 0, 0, SRCCOPY);

		DeleteObject(hBitmap);
		DeleteDC(mDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (!move && !endcheck) {
			if (wParam == VK_LEFT) {
				arrow = _LT;
			}
			else if (wParam == VK_RIGHT) {
				arrow = _RT;
			}
			else if (wParam == VK_UP) {
				arrow = _UP;
			}
			else if (wParam == VK_DOWN) {
				arrow = _DN;
			}
			if (arrow) {
				move = true;
				for (int i = 0; i < line * line; ++i)
					startingmovecheck(numblock[i]);
			}
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
		m = { LOWORD(lParam), HIWORD(lParam) };
		oldM = m;

		drag = TRUE;

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_MOUSEMOVE:
		m = { LOWORD(lParam), HIWORD(lParam) };

		if (drag) {
			if (!move && !endcheck) {
				if (m.x > oldM.x + 50) {
					arrow = _RT;
					drag = FALSE;
				}
				else if (m.x < oldM.x - 50) {
					arrow = _LT;
					drag = FALSE;
				}
				else if (m.y > oldM.y + 50) {
					arrow = _DN;
					drag = FALSE;
				}
				else if (m.y < oldM.y - 50) {
					arrow = _UP;
					drag = FALSE;
				}

				if (!drag) {
					move = true;
					for (int i = 0; i < line * line; ++i)
						startingmovecheck(numblock[i]);
				}
			}
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_LBUTTONUP:

		drag = FALSE;

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER:
		if (move) {
			bool    finchceck = true;
			POINT temp = { 0,0 };
			for (int k = 0; k < 7; ++k) {
				if (arrow == _LT) temp = { k,0 };
				else if (arrow == _RT) temp = { 6 - k,0 };
				else if (arrow == _UP) temp = { 0,k };
				else if (arrow == _DN) temp = { 0,6 - k };
				for (int i = 0; i < line * line; ++i) {
					if (temp.x == 0) {
						if (temp.y-1 == numblock[i].bpt.y) {
							for (int j = 0; j < 5; ++j) {
								movingblock(numblock[i]);
								if (numblock[i].dpt.x == 0 && numblock[i].dpt.y == 0) {
									startingmovecheck(numblock[i]);
									for (int i = 0; i < line * line; ++i) {
										for (int j = 0; j < i; ++j) {
											if (numblock[j].rpt.x == numblock[i].rpt.x && numblock[j].rpt.y == numblock[i].rpt.y && numblock[j].num == numblock[i].num) {
												numblock[j].num *= 2;
												numblock[i].bpt = { -1,-1 };
												numblock[i].dpt = { 0,0 };
												numblock[i].num = 0;
												numblock[i].rpt = { 0,0 };
											}
										}
									}
								}
								if (numblock[i].dpt.x != 0 || numblock[i].dpt.y != 0) finchceck = false;
							}
						}
					}
					else if (temp.y == 0) {
						if (temp.x-1 == numblock[i].bpt.x) {
							for (int j = 0; j < 5; ++j) {
								movingblock(numblock[i]);
								if (numblock[i].dpt.x == 0 && numblock[i].dpt.y == 0) {
									startingmovecheck(numblock[i]);
									for (int i = 0; i < line * line; ++i) {
										for (int j = 0; j < i; ++j) {
											if (numblock[j].rpt.x == numblock[i].rpt.x && numblock[j].rpt.y == numblock[i].rpt.y && numblock[j].num == numblock[i].num) {
												numblock[j].num *= 2;
												numblock[i].bpt = { -1,-1 };
												numblock[i].dpt = { 0,0 };
												numblock[i].num = 0;
												numblock[i].rpt = { 0,0 };
											}
										}
									}
								}
								if (numblock[i].dpt.x != 0 || numblock[i].dpt.y != 0) finchceck = false;
							}
						}
					}
				}
			}
			if (finchceck) {
				bool full = true;
				move = false;
				arrow = 0;
				for (int i = 0; i < line * line; ++i) {
					if (numblock[i].num != 0) {
						full = false;
						break;
					}
				}
				if (full) {
					endcheck = true;
				}
				else {
					createblock();
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			for (int i = 0; i < line * line; ++i) {
				if (endcheck) {
					KillTimer(hWnd, 1);
					MessageBox(hWnd, L"게임을 종료합니다.", L"패배", MB_OK);
					PostQuitMessage(0);
				}

				if (numblock[i].num == target) {
					KillTimer(hWnd, 1);
					MessageBox(hWnd, L"축하합니다!", L"승리", MB_OK);
					PostQuitMessage(0);
				}
			}
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_GAME_START:
			if (!start) {
				start = true;
				setting(clRect);
			}
			break;
		case ID_GAME_END:
			PostQuitMessage(0);
			break;
		case ID_TARGET_32:
			target = 32;
			break;
		case ID_TARGET_64:
			target = 64;
			break;
		case ID_OBSTACLE_2:
			obs = 2;
			break;
		case ID_OBSTACLE_3:
			obs = 3;
			break;
		case ID_OBSTACLE_4:
			obs = 4;
			break;
		}
		break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMsg, wParam, lParam));
}