#pragma comment (lib, "msimg32.lib")
#pragma comment (lib,"Gdiplus.lib")
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <atlImage.h>
#include <cmath>
#include <gdiplus.h>
using namespace Gdiplus;


#define line 4
#define startcount 2
#define boardsize 500
#define menusize 450
#define chestsize 200

#define _LT 1
#define _RT 2
#define _UP 3
#define _DN 4

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"5-2";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void CALLBACK TimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime);

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
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

int				page = 0;										// 0 : 시작화면, 1 : 이동 페이지, 2 : 전투 페이지, 3 : 이벤트 페이지, 4 : 수수께끼 페이지, 5 : 중간보스 페이지, 6 : 최종보스 페이지

// 2048 블록
struct _2048NumBlock
{
	POINT			rpt;
	POINT			bpt;
	POINT			dpt;
	int				num;
	int				size;
	int				type;										// 0 : 기본, 1 : ? 표시 2 : 선택(색깔바꾸기) 3 : ? 선택
};
// 2048 보드
struct _2048Board
{
	POINT pt;
	int width;
	int height;
	int arrtype[line][line];
};
// 몬스터
struct _mob
{
	int hp;
	int atk;
};

_2048NumBlock	numblock[line * line];							// 2048 블록
_2048Board		board;											// 2048 보드
int				_2048arrow;										// 2048 방향
bool			_2048move = false;								// 2048 움직이는 중인지
bool			attackCheck = false, defendCheck = false;
int				playeratk = 0;
int				blockcount = 0;
bool			gameover = false, gameclear = false;
int				gametime = 0, battlecount = 0, itemusecount = 0;

int				playerMove = 0, playerAttack = 0, playerDefend = 0, mobAttack = 0, MoveturnCnt = 10, playerHp = 5, battleSel = -1;
int				bossPattern = 0, bossPatternStack = 0, bossPhase = 1;
POINT			start = { 0,0 }, curtain = { 0,0 };
_mob			mob = { 6, 0 };

// 2048 보드 초기세팅
void	_2048setting(RECT rt)
{
	board.pt = { rt.right / 2 - boardsize / 2 , rt.top + 50 };
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
		numblock[i].type = 3;
	}
	for (int i = 0; i < startcount; ++i) {
		while (true) {
			int x = rand() % line;
			int y = rand() % line;
			if (board.arrtype[y][x] == 0) {
				numblock[i].bpt.x = x;
				numblock[i].bpt.y = y;
				numblock[i].num = 2;
				numblock[i].rpt.x = board.pt.x + numblock[i].size * x;
				numblock[i].rpt.y = board.pt.y + numblock[i].size * y;
				numblock[i].type = 0;
				blockcount++;
				break;
			}
		}
	}
}
// 2048 다음칸의 타입
int		_2048nextarrtype(_2048NumBlock nb)
{
	if (nb.num == 0) return 2;
	if (_2048arrow == _LT) {
		if (nb.bpt.x == 0) return 2;
		return board.arrtype[nb.bpt.y][nb.bpt.x - 1];
	}
	else if (_2048arrow == _RT) {
		if (nb.bpt.x == line - 1) return 2;
		return board.arrtype[nb.bpt.y][nb.bpt.x + 1];
	}
	else if (_2048arrow == _UP) {
		if (nb.bpt.y == 0) return 2;
		return board.arrtype[nb.bpt.y - 1][nb.bpt.x];
	}
	else if (_2048arrow == _DN) {
		if (nb.bpt.y == line - 1) return 2;
		return board.arrtype[nb.bpt.y + 1][nb.bpt.x];
	}
}
// 2048 시작전 움직임 확인
int		_2048startingmovecheck(_2048NumBlock& nb)
{
	if (nb.num == 0) return 0;
	int temp = _2048nextarrtype(nb);
	board.arrtype[nb.bpt.y][nb.bpt.x] = 1;
	if (temp == 2) return 0;
	if (_2048arrow == _LT) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x - 1 && numblock[i].bpt.y == nb.bpt.y) {
				if (numblock[i].num != nb.num)
					return 0;
				else if (numblock[i].type == 3 || nb.type == 3)
					return 0;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y][nb.bpt.x--] = 0;
		nb.dpt = { -nb.size,0 };
	}
	else if (_2048arrow == _RT) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x + 1 && numblock[i].bpt.y == nb.bpt.y) {
				if (numblock[i].num != nb.num)
					return 0;
				else if (numblock[i].type == 3 || nb.type == 3)
					return 0;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y][nb.bpt.x++] = 0;
		nb.dpt = { nb.size,0 };
	}
	else if (_2048arrow == _UP) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x && numblock[i].bpt.y == nb.bpt.y - 1) {
				if (numblock[i].num != nb.num)
					return 0;
				else if (numblock[i].type == 3 || nb.type == 3)
					return 0;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y--][nb.bpt.x] = 0;
		nb.dpt = { 0,-nb.size };
	}
	else if (_2048arrow == _DN) {
		for (int i = 0; i < line * line; ++i) {
			if (numblock[i].bpt.x == nb.bpt.x && numblock[i].bpt.y == nb.bpt.y + 1) {
				if (numblock[i].num != nb.num)
					return 0;
				else if (numblock[i].type == 3 || nb.type == 3)
					return 0;
				else {
					break;
				}
			}
		}
		board.arrtype[nb.bpt.y++][nb.bpt.x] = 0;
		nb.dpt = { 0,nb.size };
	}
	return 1;
}
// 2048 이동중 확인
void	_2048movingblock(_2048NumBlock& nb)
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
// 2048 새로운 블록 만들기
void	_2048createblock()
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
					numblock[i].type = 0;
					blockcount++;
					break;
				}
			}
			break;
		}
	}
}
// 2048 블럭의 위치를 서로 바꾸기
void	_2048blockchange()
{
	int count = 0;
	for (int i = 0; i < line * line; ++i) {
		if (numblock[i].num != 0) {
			count++;
		}
	}
	if (count <= 1) return;

	_2048NumBlock* num1;
	_2048NumBlock* num2;
	int temp;
	while (true) {
		num1 = &numblock[rand() % (line * line)];
		num2 = &numblock[rand() % (line * line)];
		if (num1->num != 0 && num2->num != 0) {
			temp = num1->num;
			num1->num = num2->num;
			num2->num = temp;
			break;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC, mDC;
	PAINTSTRUCT ps;
	static TCHAR showTurn[20], t_gametime[20], t_battlecount[20], t_itemusecount[20], t_quitmessage[20];
	static HBITMAP hBitmap;
	static RECT clRect, nextMap[3], answer[3], battle[3], question, startbtm;

	static CImage _2048[12];
	static CImage _2048marked[12];
	static CImage _player[4];
	static CImage _map[4];
	static CImage _chest[3];
	static CImage _icon[18];
	static CImage _question[10];
	static CImage _background[5];
	static CImage _mob[6];
	static CImage _playeratk[4];
	static CImage _title;
	static CImage _item_info[3];
	static CImage _gameclear;
	static CImage _gameover;

	static POINT m, player, menu, chest; // 마우스, 플레이어, 메뉴창, 상자 위치
	static int moveCheck, nextSel, answerSel, randQ; // 2048 못움직이는거 확인, 다음 스테이지 선택, 답 선택, 문제 선택
	static int stage;
	static bool chestcheck = false, chestcurse, chestnothing = false;
	static RECT eventcheck, yes, no;
	static bool items[3], itemUse[3];

	switch (iMsg) {
	case WM_CREATE:
		GetClientRect(hWnd, &clRect);
		_2048[0].Load(_T("2048\\2048stone_2.png"));
		_2048[1].Load(_T("2048\\2048stone_4.png"));
		_2048[2].Load(_T("2048\\2048stone_8.png"));
		_2048[3].Load(_T("2048\\2048stone_16.png"));
		_2048[4].Load(_T("2048\\2048stone_32.png"));
		_2048[5].Load(_T("2048\\2048stone_64.png"));
		_2048[6].Load(_T("2048\\2048stone_128.png"));
		_2048[7].Load(_T("2048\\2048stone_256.png"));
		_2048[8].Load(_T("2048\\2048stone_512.png"));
		_2048[9].Load(_T("2048\\2048stone_1024.png"));
		_2048[10].Load(_T("2048\\2048stone_2048.png"));
		_2048[11].Load(_T("2048\\2048stone_QM.png"));
		_2048marked[0].Load(_T("2048\\2048stone_marked_2.png"));
		_2048marked[1].Load(_T("2048\\2048stone_marked_4.png"));
		_2048marked[2].Load(_T("2048\\2048stone_marked_8.png"));
		_2048marked[3].Load(_T("2048\\2048stone_marked_16.png"));
		_2048marked[4].Load(_T("2048\\2048stone_marked_32.png"));
		_2048marked[5].Load(_T("2048\\2048stone_marked_64.png"));
		_2048marked[6].Load(_T("2048\\2048stone_marked_128.png"));
		_2048marked[7].Load(_T("2048\\2048stone_marked_256.png"));
		_2048marked[8].Load(_T("2048\\2048stone_marked_512.png"));
		_2048marked[9].Load(_T("2048\\2048stone_marked_1024.png"));
		_2048marked[10].Load(_T("2048\\2048stone_marked_2048.png"));
		_2048marked[11].Load(_T("2048\\2048stone_marked_QM.png"));
		_player[0].Load(_T("player\\player_idle.gif"));
		_player[1].Load(_T("player\\player_walk.gif"));
		_player[2].Load(_T("player\\player_attack.png"));
		_player[3].Load(_T("player\\player_defend.png"));
		_map[0].Load(_T("map\\stage1.png"));
		_map[1].Load(_T("map\\stage2.png"));
		_map[2].Load(_T("map\\stage3.png"));
		_map[3].Load(_T("map\\stage4.png"));
		_icon[0].Load(_T("icon\\monster.png"));
		_icon[1].Load(_T("icon\\chest.png"));
		_icon[2].Load(_T("icon\\sphinx.png"));
		_icon[3].Load(_T("icon\\boss.png"));
		_icon[4].Load(_T("icon\\a.png"));
		_icon[5].Load(_T("icon\\b.png"));
		_icon[6].Load(_T("icon\\c.png"));
		_icon[7].Load(_T("icon\\heart.png"));
		_icon[8].Load(_T("icon\\potion.png"));
		_icon[9].Load(_T("icon\\potion_black.png"));
		_icon[10].Load(_T("icon\\shield.png"));
		_icon[11].Load(_T("icon\\shield_black.png"));
		_icon[12].Load(_T("icon\\scouter.png"));
		_icon[13].Load(_T("icon\\scouter_black.png"));
		_icon[14].Load(_T("icon\\attack.png"));
		_icon[15].Load(_T("icon\\defend.png"));
		_icon[16].Load(_T("icon\\item.png"));
		_icon[17].Load(_T("icon\\return.png"));
		_question[0].Load(_T("question\\question1.png"));
		_question[1].Load(_T("question\\question2.png"));
		_question[2].Load(_T("question\\question3.png"));
		_question[3].Load(_T("question\\question4.png"));
		_question[4].Load(_T("question\\question5.png"));
		_question[5].Load(_T("question\\question6.png"));
		_question[6].Load(_T("question\\question7.png"));
		_question[7].Load(_T("question\\question8.png"));
		_question[8].Load(_T("question\\question9.png"));
		_question[9].Load(_T("question\\question10.png"));
		_background[0].Load(_T("background\\2048board.png"));
		_background[1].Load(_T("background\\menu.png"));
		_background[2].Load(_T("background\\status.png"));
		_background[3].Load(_T("background\\text.png"));
		_background[4].Load(_T("background\\curtain.png"));
		_chest[0].Load(_T("chest\\closedchest.png"));
		_chest[1].Load(_T("chest\\opendchest.png"));
		_chest[2].Load(_T("chest\\opendcursechest.png"));
		_mob[0].Load(_T("mob\\1stagemob.png"));
		_mob[1].Load(_T("mob\\1stagemob_atk.png"));
		_mob[2].Load(_T("mob\\2stagemob.png"));
		_mob[3].Load(_T("mob\\2stagemob_atk.png"));
		_mob[4].Load(_T("mob\\3stagemob.png"));
		_mob[5].Load(_T("mob\\3stagemob_atk.png"));
		_playeratk[0].Load(_T("playeratk\\playeratk_1.png"));
		_playeratk[1].Load(_T("playeratk\\playeratk_2.png"));
		_playeratk[2].Load(_T("playeratk\\playeratk_3.png"));
		_playeratk[3].Load(_T("playeratk\\playeratk_4.png"));
		_title.Load(_T("map\\title.png"));
		_item_info[0].Load(_T("item_info\\item_info1.png"));
		_item_info[1].Load(_T("item_info\\item_info2.png"));
		_item_info[2].Load(_T("item_info\\item_info3.png"));
		_gameclear.Load(_T("background\\victory.png"));
		_gameover.Load(_T("background\\defeat.png"));

		start = { 0,0 };
		player.x = clRect.right / 2 - boardsize / 2 - 200;
		player.y = clRect.top + boardsize + 100;
		menu.x = clRect.right / 2 - menusize / 2;
		menu.y = clRect.top + boardsize + 70;
		nextMap[1] = { clRect.right / 2 - 20, menu.y + ((menusize - 50) / 2) - 20, clRect.right / 2 + 20, menu.y + ((menusize - 50) / 2) + 20 };
		nextMap[0] = { menu.x + 20, nextMap[1].top, menu.x + 60, nextMap[1].bottom };
		nextMap[2] = { menu.x + menusize - 60, nextMap[1].top, menu.x + menusize - 20, nextMap[1].bottom };

		answer[0] = { nextMap[0].left, menu.y + menusize - 65, nextMap[0].right, menu.y + menusize - 25 };
		answer[1] = { nextMap[1].left, menu.y + menusize - 65, nextMap[1].right, menu.y + menusize - 25 };
		answer[2] = { nextMap[2].left, menu.y + menusize - 65, nextMap[2].right, menu.y + menusize - 25 };

		battle[1] = { clRect.right / 2 - 30, menu.y + ((menusize - 50) / 2) - 30, clRect.right / 2 + 30, menu.y + ((menusize - 50) / 2) + 30 };
		battle[0] = { menu.x + 5, battle[1].top, menu.x + 65 ,battle[1].bottom };
		battle[2] = { menu.x + menusize - 65, battle[1].top, menu.x + menusize - 5 ,battle[1].bottom };

		startbtm = { clRect.right / 2 - 170, clRect.bottom - 170, clRect.right / 2 + 170, clRect.bottom - 90 };

		question = { menu.x, menu.y, menu.x + 450, menu.y + 300 };

		playerMove = moveCheck = 0;
		stage = 1;
		nextSel = answerSel = -1;
		MoveturnCnt = 10;
		srand(time(NULL));
		_2048setting(clRect);
		for (int i = 0; i < 3; ++i) items[i] = itemUse[i] = false;
		SetTimer(hWnd, 1, 10, (TIMERPROC)TimerProc);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		mDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, clRect.right, clRect.bottom);

		SelectObject(mDC, (HBITMAP)hBitmap);

		FillRect(mDC, &clRect, (HBRUSH)GetStockObject(BLACK_BRUSH)); // 맵 바탕
		if (page != 0) {
			if (page == 5) _map[3].Draw(mDC, 0, 0, clRect.right, clRect.bottom, start.x, 0, _map[3].GetWidth(), _map[3].GetHeight()); // 맵 그리기 (page, stage에 따라 달라짐)
			else if (stage >= 11) _map[2].Draw(mDC, 0, 0, clRect.right, clRect.bottom, start.x, 0, _map[2].GetWidth(), _map[2].GetHeight());
			else if (stage >= 6) _map[1].Draw(mDC, 0, 0, clRect.right, clRect.bottom, start.x, 0, _map[1].GetWidth(), _map[1].GetHeight());
			else if (stage >= 1) _map[0].Draw(mDC, 0, 0, clRect.right, clRect.bottom, start.x, 0, _map[0].GetWidth(), _map[0].GetHeight());

			_background[0].Draw(mDC, board.pt.x - 20, board.pt.y - 20, boardsize + 40, boardsize + 40, 0, 0, _background[0].GetWidth(), _background[0].GetHeight()); // 2048 배경
			_background[2].Draw(mDC, board.pt.x - 500, board.pt.y + 20, 400, 450, 0, 0, _background[2].GetWidth(), _background[2].GetHeight()); // 상태 창

			SetBkMode(mDC, TRANSPARENT);
			SetTextColor(mDC, RGB(255, 255, 255));

			// 상태 창 내용 (스테이지, 턴, 체력, 아이템)

			wsprintf(showTurn, L"%d 스테이지", stage);
			TextOut(mDC, board.pt.x - 470, board.pt.y + 50, showTurn, lstrlen(showTurn));
			if (page == 2 || page == 5) {
				wsprintf(showTurn, L"다음 행동까지 %d 턴", MoveturnCnt);
			}
			else {
				wsprintf(showTurn, L"다음 스테이지 선택까지 %d 턴", MoveturnCnt);
			}
			TextOut(mDC, board.pt.x - 470, board.pt.y + 90, showTurn, lstrlen(showTurn));

			for (int i = 0; i < playerHp; ++i) {
				_icon[7].Draw(mDC, board.pt.x - 470 + 40 * i, board.pt.y + 125, 40, 40, 0, 0, _icon[7].GetWidth(), _icon[7].GetHeight());
			}

			if (items[0]) _icon[8].Draw(mDC, board.pt.x - 470, board.pt.y + 180, 80, 80, 0, 0, _icon[8].GetWidth(), _icon[8].GetHeight());
			else _icon[9].Draw(mDC, board.pt.x - 470, board.pt.y + 180, 80, 80, 0, 0, _icon[9].GetWidth(), _icon[9].GetHeight());
			_item_info[0].Draw(mDC, board.pt.x - 390, board.pt.y + 190, 270, 60, 0, 0, _item_info[0].GetWidth(), _item_info[0].GetHeight());
			if (items[1]) _icon[10].Draw(mDC, board.pt.x - 470, board.pt.y + 270, 80, 80, 0, 0, _icon[10].GetWidth(), _icon[10].GetHeight());
			else _icon[11].Draw(mDC, board.pt.x - 470, board.pt.y + 270, 80, 80, 0, 0, _icon[11].GetWidth(), _icon[11].GetHeight());
			_item_info[1].Draw(mDC, board.pt.x - 390, board.pt.y + 280, 270, 60, 0, 0, _item_info[1].GetWidth(), _item_info[1].GetHeight());
			if (items[2]) _icon[12].Draw(mDC, board.pt.x - 470, board.pt.y + 360, 80, 80, 0, 0, _icon[12].GetWidth(), _icon[12].GetHeight());
			else _icon[13].Draw(mDC, board.pt.x - 470, board.pt.y + 360, 80, 80, 0, 0, _icon[13].GetWidth(), _icon[13].GetHeight());
			_item_info[2].Draw(mDC, board.pt.x - 390, board.pt.y + 370, 270, 60, 0, 0, _item_info[2].GetWidth(), _item_info[2].GetHeight());

			SetTextColor(mDC, RGB(0, 0, 0));
			// 2048 격자무늬 그리기

			for (int i = 0; i < line + 1; ++i) {
				MoveToEx(mDC, board.pt.x, board.pt.y + (board.height / line) * i, NULL);
				LineTo(mDC, board.pt.x + board.width, board.pt.y + (board.height / line) * i);
				MoveToEx(mDC, board.pt.x + (board.width / line) * i, board.pt.y, NULL);
				LineTo(mDC, board.pt.x + (board.width / line) * i, board.pt.y + board.height);
			}

			for (int i = 0; i < line * line; ++i) {
				if (numblock[i].num > 0) {
					if (numblock[i].type == 0)
						_2048[(int)(log2(numblock[i].num) - 1)].Draw(mDC, numblock[i].rpt.x, numblock[i].rpt.y, numblock[i].size, numblock[i].size, 0, 0, _2048[0].GetWidth(), _2048[0].GetHeight());
					else if (numblock[i].type == 1)
						_2048[11].Draw(mDC, numblock[i].rpt.x, numblock[i].rpt.y, numblock[i].size, numblock[i].size, 0, 0, _2048[11].GetWidth(), _2048[11].GetHeight());
					else if (numblock[i].type == 2)
						_2048marked[(int)(log2(numblock[i].num) - 1)].Draw(mDC, numblock[i].rpt.x, numblock[i].rpt.y, numblock[i].size, numblock[i].size, 0, 0, _2048[0].GetWidth(), _2048[0].GetHeight());
					else if (numblock[i].type == 3)
						_2048marked[11].Draw(mDC, numblock[i].rpt.x, numblock[i].rpt.y, numblock[i].size, numblock[i].size, 0, 0, _2048[11].GetWidth(), _2048[11].GetHeight());
				}
			}
		}
		// 시작 페이지
		if (page == 0) {
			_title.Draw(mDC, 0, 0, clRect.right, clRect.bottom, start.x, 0, _title.GetWidth(), _title.GetHeight());
		}

		// 이동 페이지
		if (page == 1) {
			if (playerMove) _player[1].Draw(mDC, player.x, player.y, _player[1].GetWidth(), _player[1].GetHeight());
			else _player[0].Draw(mDC, player.x, player.y, _player[0].GetWidth(), _player[0].GetHeight());
			if (!MoveturnCnt) {
				_background[1].Draw(mDC, menu.x, menu.y, menusize, menusize, 0, 0, _background[1].GetWidth(), _background[1].GetHeight());
				if (stage == 14) // 다음 스테이지가 15가 아니라면 몬스터, 상자, ?를 고정으로 출력, 아니라면 보스 이미지로 출력
					for (int i = 0; i < 3; i++) _icon[3].Draw(mDC, nextMap[i]);
				else
					for (int i = 0; i < 3; i++) _icon[i].Draw(mDC, nextMap[i]);
			}
		}

		// 전투 && 보스 페이지
		if (page == 2 || page == 5) {
			if (itemUse[0]) _icon[8].Draw(mDC, player.x, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[8].GetWidth(), _icon[8].GetHeight());
			else _icon[9].Draw(mDC, player.x, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[9].GetWidth(), _icon[9].GetHeight());

			if (itemUse[1]) _icon[10].Draw(mDC, player.x + 60, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[10].GetWidth(), _icon[10].GetHeight());
			else _icon[11].Draw(mDC, player.x + 60, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[11].GetWidth(), _icon[11].GetHeight());

			if (itemUse[2]) _icon[12].Draw(mDC, player.x + 120, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[12].GetWidth(), _icon[12].GetHeight());
			else _icon[13].Draw(mDC, player.x + 120, player.y + _player[0].GetHeight() + 10, 50, 50, 0, 0, _icon[13].GetWidth(), _icon[13].GetHeight());

			if (bossPattern) {
				_background[4].Draw(mDC, curtain.x, curtain.y, boardsize + 40, boardsize + 80, 0, 0, _background[4].GetWidth(), _background[4].GetHeight()); // ���� ���� ���̶�� Ŀư ǥ��
			}

			if (playerAttack) {
				_player[2].Draw(mDC, player.x, player.y + 30, _player[2].GetWidth() / 2, _player[2].GetHeight() / 2); // 플레이어 공격 모션
			}
			else if (playerDefend) {
				_player[3].Draw(mDC, player.x, player.y, _player[0].GetWidth(), _player[0].GetHeight()); // 플레이어 방어 모션
			}
			else if (mobAttack % 10 == 1 && !attackCheck) { // 깜빡이는 거 표현.

			}
			else if (mobAttack % 10 == 1 && !defendCheck) { // 깜빡이는 거 표현.

			}
			else if (mobAttack && !defendCheck) { // 플레이어 방어 모션
				_player[3].Draw(mDC, player.x, player.y, _player[0].GetWidth(), _player[0].GetHeight());
			}
			else
				_player[0].Draw(mDC, player.x, player.y, _player[0].GetWidth(), _player[0].GetHeight());

			SetTextColor(mDC, RGB(255, 255, 255));

			if (battleSel < 3)_background[3].Draw(mDC, clRect.right / 2 + 350, player.y - 100, 250, 150);

			if (itemUse[2]) {
				wsprintf(showTurn, L"%d", mob.atk);
				TextOut(mDC, clRect.right / 2 + 500, player.y - 50, showTurn, lstrlen(showTurn));
			}

			if (stage == 15) {
				TextOut(mDC, clRect.right / 2 + 480, player.y - 50, L"?", _tcsclen(L"?"));
			}

			if (stage >= 11 && battleSel <= 3 && playeratk >= 0) {
				if (!itemUse[2]) TextOut(mDC, clRect.right / 2 + 450, player.y - 50, L"32 ~ 512", _tcsclen(L"32 ~ 512"));
				if (!mobAttack) _mob[4].Draw(mDC, clRect.right / 2 + 300, player.y + (_player[0].GetHeight() - _mob[4].GetHeight() * 2), _mob[4].GetWidth() * 5 / 2, _mob[4].GetHeight() * 5 / 2);
				else _mob[5].Draw(mDC, clRect.right / 2 + 330, player.y + (_player[0].GetHeight() - _mob[5].GetHeight() * 2 - 70), _mob[5].GetWidth() * 3, _mob[5].GetHeight() * 3);
			}
			else if (stage >= 6 && battleSel <= 3 && playeratk >= 0) {
				if (!itemUse[2]) TextOut(mDC, clRect.right / 2 + 450, player.y - 50, L"8 ~ 128", _tcsclen(L"8 ~ 128"));
				if (!mobAttack) _mob[2].Draw(mDC, clRect.right / 2 + 350, player.y + (_player[0].GetHeight() - _mob[2].GetHeight() * 2), _mob[2].GetWidth() * 2, _mob[2].GetHeight() * 2);
				else _mob[3].Draw(mDC, clRect.right / 2 + 280, player.y + (_player[0].GetHeight() - _mob[3].GetHeight() * 2 - 20), _mob[3].GetWidth() * 2, _mob[3].GetHeight() * 2);
			}
			else if (stage >= 1 && battleSel <= 3 && playeratk >= 0) {
				if (!itemUse[2]) TextOut(mDC, clRect.right / 2 + 450, player.y - 50, L"2 ~ 32", _tcsclen(L"2 ~ 32"));
				if (!mobAttack) _mob[0].Draw(mDC, clRect.right / 2 + 300, player.y + (_player[0].GetHeight() - _mob[0].GetHeight() * 2), _mob[0].GetWidth() * 2, _mob[0].GetHeight() * 2);
				else _mob[1].Draw(mDC, clRect.right / 2 + 300, player.y + (_player[0].GetHeight() - _mob[1].GetHeight() * 2), _mob[1].GetWidth() * 2, _mob[1].GetHeight() * 2);
			}

			eventcheck = { menu.x + menusize / 2 - 75,menu.y + menusize - 105,menu.x + menusize / 2 + 75,menu.y + menusize - 30 };

			SetTextColor(mDC, RGB(0, 0, 0));

			if (!MoveturnCnt && !playerAttack && !playerDefend && !mobAttack && !bossPattern) {
				_background[1].Draw(mDC, menu.x, menu.y, menusize, menusize, 0, 0, _background[1].GetWidth(), _background[1].GetHeight());
				if (battleSel < 0) {
					_icon[14].Draw(mDC, battle[0]);
					_icon[15].Draw(mDC, battle[1]);
					_icon[16].Draw(mDC, battle[2]);
				}
				else if (battleSel == 0 || battleSel == 1) {
					_icon[17].Draw(mDC, eventcheck);
				}
				else if (battleSel == 2) {
					if (items[0]) _icon[8].Draw(mDC, battle[0]);
					else _icon[9].Draw(mDC, battle[0]);
					if (items[1]) _icon[10].Draw(mDC, battle[1]);
					else _icon[11].Draw(mDC, battle[1]);
					if (items[2]) _icon[12].Draw(mDC, battle[2]);
					else _icon[13].Draw(mDC, battle[2]);
					_icon[17].Draw(mDC, eventcheck);
				}
				else if (battleSel == 3) {
					if (playeratk >= 0) {
						playeratk--;
						int temp = playeratk / 10;
						_playeratk[temp].Draw(mDC, clRect.right / 2 + 300, player.y + (_player[0].GetHeight() - _mob[4].GetHeight() * 2), _mob[0].GetWidth() * 2, _mob[0].GetHeight() * 2);
					}
					if (playeratk < 0) {
						TextOut(mDC, menu.x + 200, menu.y + 35 + chestsize, L"승리!", _tcsclen(L"승리!"));
						_icon[17].Draw(mDC, eventcheck);
					}
				}
				else if (battleSel == 4) {
					TextOut(mDC, menu.x + 130, menu.y + 35 + chestsize, L"몬스터가 도망갔습니다.", _tcsclen(L"몬스터가 도망갔습니다."));
					_icon[17].Draw(mDC, eventcheck);
				}
			}
		}

		// 이벤트 페이지
		if (page == 3) {
			_background[1].Draw(mDC, menu.x, menu.y, menusize, menusize, 0, 0, _background[1].GetWidth(), _background[1].GetHeight());
			if (chestcheck) {
				if (chestcurse) {
					_chest[2].Draw(mDC, menu.x + menusize / 2 - chestsize / 2, menu.y + 5, chestsize, chestsize, 0, 0, _chest[2].GetWidth(), _chest[2].GetHeight());
					TextOut(mDC, menu.x + 120, menu.y + 35 + chestsize, L"저주받은 상자를 열었습니다.", _tcsclen(L"저주받은 상자를 열었습니다."));
					if (chestnothing) TextOut(mDC, menu.x + 80, menu.y + 65 + chestsize, L"그러나 아무 일도 일어나지 않았습니다.", _tcsclen(L"그러나 아무 일도 일어나지 않았습니다."));
					else TextOut(mDC, menu.x + 130, menu.y + 65 + chestsize, L"아이템을 하나 잃습니다.", _tcsclen(L"아이템을 하나 잃습니다."));
				}
				else {
					_chest[1].Draw(mDC, menu.x + menusize / 2 - chestsize / 2, menu.y + 5, chestsize, chestsize, 0, 0, _chest[1].GetWidth(), _chest[1].GetHeight());
					TextOut(mDC, menu.x + 130, menu.y + 35 + chestsize, L"보물 상자를 열었습니다.", _tcsclen(L"보물 상자를 열었습니다."));
					if (chestnothing) TextOut(mDC, menu.x + 100, menu.y + 65 + chestsize, L"그러나 그 안은 텅 비어있었습니다.", _tcsclen(L"그러나 그 안은 텅 비어있었습니다."));
					else TextOut(mDC, menu.x + 130, menu.y + 65 + chestsize, L"아이템을 하나 얻습니다.", _tcsclen(L"아이템을 하나 얻습니다."));
				}
				eventcheck = { menu.x + menusize / 2 - 75,menu.y + menusize - 105,menu.x + menusize / 2 + 75,menu.y + menusize - 30 };
				_icon[17].Draw(mDC, eventcheck);
			}
			else {
				_chest[0].Draw(mDC, menu.x + menusize / 2 - chestsize / 2, menu.y + 5, chestsize, chestsize, 0, 0, _chest[0].GetWidth(), _chest[0].GetHeight());
				yes = { menu.x + menusize / 2 - chestsize / 2, menu.y + 5, menu.x + menusize / 2 - chestsize / 2 + chestsize, menu.y + 5 + chestsize };
				no = { menu.x + menusize / 2 - 75,menu.y + menusize - 105,menu.x + menusize / 2 + 75,menu.y + menusize - 30 };
				_icon[17].Draw(mDC, no);
			}
		}

		// 스핑크스 페이지
		if (page == 4) {
			_background[1].Draw(mDC, menu.x, menu.y, menusize, menusize, 0, 0, _background[1].GetWidth(), _background[1].GetHeight());
			if (answerSel < 0) {
				_question[randQ].Draw(mDC, question);
				for (int i = 0; i < 3; i++) _icon[i + 4].Draw(mDC, answer[i]);
			}
			else {
				if (answerSel == 2) {
					TextOut(mDC, menu.x + 200, menu.y + 35 + chestsize, L"정답!", _tcsclen(L"정답!"));
					if (chestnothing) TextOut(mDC, menu.x + 80, menu.y + 65 + chestsize, L"그러나 아무 일도 일어나지 않았습니다.", _tcsclen(L"그러나 아무 일도 일어나지 않았습니다."));
					else TextOut(mDC, menu.x + 130, menu.y + 65 + chestsize, L"아이템을 하나 얻습니다.", _tcsclen(L"아이템을 하나 얻습니다."));
				}
				else {
					TextOut(mDC, menu.x + 200, menu.y + 35 + chestsize, L"오답!", _tcsclen(L"오답!"));
					TextOut(mDC, menu.x + 130, menu.y + 65 + chestsize, L"체력을 한 칸 잃습니다.", _tcslen(L"체력을 한 칸 잃습니다."));
				}
				eventcheck = { menu.x + menusize / 2 - 75, menu.y + menusize - 105,menu.x + menusize / 2 + 75,menu.y + menusize - 30 };
				_icon[17].Draw(mDC, eventcheck);
			}
		}
		if (gameclear) {
			KillTimer(hWnd, 1);
			Graphics graphics(mDC);
			SolidBrush semiTransparentBrush(Color(200, 0, 0, 0));
			HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("맑은 고딕"));
			HFONT hOldFont = (HFONT)SelectObject(mDC, hFont);
			SetTextColor(mDC, RGB(255, 255, 255));
			SetBkMode(mDC, TRANSPARENT);
			graphics.FillRectangle(&semiTransparentBrush, 0, 0, clRect.right, clRect.bottom);
			_gameclear.Draw(mDC, clRect.right / 2 - 300, 0, 600, clRect.bottom, 0, 0, _gameclear.GetWidth(), _gameclear.GetHeight());
			wsprintf(t_gametime, L"총 게임시간 : %d분 %d초 %d", gametime / 6000, (gametime % 6000) / 100, gametime % 100);
			TextOut(mDC, clRect.right / 2 - 200, 300, t_gametime, lstrlen(t_gametime));
			wsprintf(t_battlecount, L"총 전투 수 : %d", battlecount);
			TextOut(mDC, clRect.right / 2 - 200, 400, t_battlecount, lstrlen(t_battlecount));
			wsprintf(t_itemusecount, L"아이템 사용 횟수 : %d", itemusecount);
			TextOut(mDC, clRect.right / 2 - 200, 500, t_itemusecount, lstrlen(t_itemusecount));
			wsprintf(t_quitmessage, L"Q를 누르시면 나가실 수 있습니다.");
			TextOut(mDC, clRect.right / 2 - 200, 600, t_quitmessage, lstrlen(t_quitmessage));
			SelectObject(mDC, hOldFont);
			DeleteObject(hFont);
		}
		else if (gameover) {
			KillTimer(hWnd, 1);
			Graphics graphics(mDC);
			SolidBrush semiTransparentBrush(Color(200, 0, 0, 0));
			HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("맑은 고딕"));
			HFONT hOldFont = (HFONT)SelectObject(mDC, hFont);
			SetTextColor(mDC, RGB(255, 255, 255));
			SetBkMode(mDC, TRANSPARENT);
			graphics.FillRectangle(&semiTransparentBrush, 0, 0, clRect.right, clRect.bottom);
			_gameover.Draw(mDC, clRect.right / 2 - 300, 0, 600, clRect.bottom, 0, 0, _gameover.GetWidth(), _gameover.GetHeight());
			wsprintf(t_gametime, L"총 게임시간 : %d분 %d초 %d", gametime / 6000, (gametime % 6000) / 100, gametime % 100);
			TextOut(mDC, clRect.right / 2 - 200, 300, t_gametime, lstrlen(t_gametime));
			wsprintf(t_battlecount, L"총 전투 수 : %d", battlecount);
			TextOut(mDC, clRect.right / 2 - 200, 400, t_battlecount, lstrlen(t_battlecount));
			wsprintf(t_itemusecount, L"아이템 사용 횟수 : %d", itemusecount);
			TextOut(mDC, clRect.right / 2 - 200, 500, t_itemusecount, lstrlen(t_itemusecount));
			wsprintf(t_quitmessage, L"Q를 누르시면 나가실 수 있습니다.");
			TextOut(mDC, clRect.right / 2 - 200, 600, t_quitmessage, lstrlen(t_quitmessage));
			SelectObject(mDC, hOldFont);
			DeleteObject(hFont);
		}

		BitBlt(hDC, 0, 0, clRect.right, clRect.bottom, mDC, 0, 0, SRCCOPY);

		DeleteObject(hBitmap);
		DeleteDC(mDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (gameclear || gameover) {
			if (wParam == 'Q') PostQuitMessage(0);
		}
		else {
			if (!_2048move && !playerMove && MoveturnCnt) {
				if (wParam == VK_LEFT) {
					_2048arrow = _LT;
				}
				else if (wParam == VK_RIGHT) {
					_2048arrow = _RT;
				}
				else if (wParam == VK_UP) {
					_2048arrow = _UP;
				}
				else if (wParam == VK_DOWN) {
					_2048arrow = _DN;
				}
				if (_2048arrow) {
					moveCheck = 0;
					for (int i = 0; i < line * line; ++i)
						moveCheck += _2048startingmovecheck(numblock[i]);
					if (moveCheck) {
						if (page == 1) playerMove = 50;
						_2048move = true;
					}
				}
			}
		}
		break;

	case WM_LBUTTONDOWN:
		m = { LOWORD(lParam), HIWORD(lParam) };
		if (page == 0) {
			if (PtInRect(&startbtm, m)) {
				page = 1;
			}
		}
		else if (page == 1) {
			if (!MoveturnCnt && nextSel < 0) {
				for (int i = 0; i < 3; i++) {
					if (PtInRect(&nextMap[i], m)) {
						nextSel = i;
						stage++;
						break;
					}
				}
			}

			if (stage == 14) nextSel = 3;
			switch (nextSel) {
			case 0: // 몬스터
				// 몬스터 프리셋 가져오기
				if (stage >= 11) mob.atk = pow(2, rand() % 5 + 5);
				else if (stage >= 6) mob.atk = pow(2, rand() % 5 + 3);
				else if (stage >= 1) mob.atk = pow(2, rand() % 5 + 1);
				page = 2; // 전투페이지 돌입
				MoveturnCnt = 5;
				battlecount++;
				break;
			case 1: // 상자
				page = 3; // 이벤트 && 상자로 이동
				chestcheck = false;
				break;
			case 2: // 스핑크스
				page = 4; // 이벤트 && 스핑크스로 이동
				randQ = rand() % 10;
				answerSel = -1;
				break;
			case 3: // 보스
				page = 5;
				mob.hp = 6;
				mob.atk = pow(2, rand() % 4 + 6);
				break;
			}
		}
		else if (page == 2 || page == 5) { // 일반 몬스터 페이지
			if (battleSel < 0) {
				for (int i = 0; i < 3; ++i) {
					if (PtInRect(&battle[i], m)) {
						battleSel = i;
					}
				}
			}
			else {
				switch (battleSel) {
				case 0:
					for (int i = 0; i < line * line; ++i) {
						if (m.x > numblock[i].rpt.x && m.x < numblock[i].rpt.x + numblock[i].size && m.y > numblock[i].rpt.y && m.y < numblock[i].rpt.y + numblock[i].size) {
							if (itemUse[0]) {
								itemUse[0] = false;
								if (numblock[i].num * 2 > mob.atk) {
									playerAttack = 10; // 공격 애니메이션 시간
									attackCheck = defendCheck = true;
								}
								else {
									playerAttack = 50; // 공격 실패 애니메이션 시간
									attackCheck = false;
								}
							}
							else {
								if (numblock[i].num > mob.atk) {
									playerAttack = 50; // 공격 애니메이션 시간
									attackCheck = defendCheck = true;
								}
								else {
									playerAttack = 50; // 공격 실패 애니메이션 시간
									attackCheck = false;
								}
							}
						}
					}

					if (PtInRect(&eventcheck, m)) {
						battleSel = -1;
					}
					break;
				case 1:
					for (int i = 0; i < line * line; ++i) {
						if (m.x > numblock[i].rpt.x && m.x < numblock[i].rpt.x + numblock[i].size && m.y > numblock[i].rpt.y && m.y < numblock[i].rpt.y + numblock[i].size) {
							if (numblock[i].num >= mob.atk / 2) {
								playerDefend = 50;// 방어 애니메이션 시간
								attackCheck = defendCheck = true;
								if (page == 2 || (page == 5 && mob.atk >= 4))mob.atk /= 2;
							}
							else {
								if (itemUse[1]) {
									playerDefend = 50; // 방어 애니메이션 시간
									attackCheck = defendCheck = true;
									itemUse[1] = false;
								}
								else {
									playerDefend = 50; // 방어 실패 애니메이션 시간
									defendCheck = false;
								}
							}
							numblock[i].bpt = { -1,-1 };
							numblock[i].dpt = { 0,0 };
							numblock[i].num = 0;
							numblock[i].rpt = { 0,0 };
							numblock[i].type = 0;
						}
					}

					if (PtInRect(&eventcheck, m)) {
						battleSel = -1;
					}
					break;
				case 2:
					for (int i = 0; i < 3; ++i) {
						if (PtInRect(&battle[i], m)) {
							if (items[i]) {
								itemUse[i] = true;
								items[i] = false;
								itemusecount++;
							}
						}
					}
					if (PtInRect(&eventcheck, m)) {
						battleSel = -1;
					}
					break;
				case 3: // 일반 몬스터 처치 시
					if (PtInRect(&eventcheck, m)) {
						page = 1;
						playeratk = 0;
						for (int i = 0; i < 3; ++i) itemUse[i] = false;
						nextSel = battleSel = -1;
						start.x = 0;
						MoveturnCnt = 10;
					}
					break;
				case 4: // 일반 몬스터 도망 시
					if (PtInRect(&eventcheck, m)) {
						page = 1;
						for (int i = 0; i < 3; ++i) itemUse[i] = false;
						nextSel = battleSel = -1;
						start.x = 0;
						MoveturnCnt = 10;
					}
					break;
				}
			}
		}
		else if (page == 3) { // 이벤트 페이지
			if (chestcheck) {
				if (PtInRect(&eventcheck, m)) {
					page = 1;
					nextSel = -1;
					start.x = 0;
					MoveturnCnt = 10;
				}
			}
			else {
				if (PtInRect(&yes, m)) {
					int ran = rand() % 2;
					chestcheck = true;
					if (ran == 0) {
						chestcurse = false;
						chestnothing = true;
						for (int i = 0; i < 3; ++i) {
							if (!items[i])
								chestnothing = false;
						}
						if (!chestnothing) {
							while (true) {
								int itemran = rand() % 3;
								if (!items[itemran]) {
									items[itemran] = true;
									break;
								}
							}
						}
					}
					else {
						chestcurse = true;
						chestnothing = true;
						for (int i = 0; i < 3; ++i) {
							if (items[i])
								chestnothing = false;
						}
						if (!chestnothing) {
							while (true) {
								int itemran = rand() % 3;
								if (items[itemran]) {
									items[itemran] = false;
									break;
								}
							}
						}
					}
				}
				else if (PtInRect(&no, m)) {
					page = 1;
					nextSel = -1;
					start.x = 0;
					MoveturnCnt = 10;
				}
			}
		}
		else if (page == 4) { // 스핑크스 페이지
			if (answerSel < 0) {
				for (int i = 0; i < 3; i++) {
					if (PtInRect(&answer[i], m))
						answerSel = i;
				}
				if (answerSel == 2) {
					chestnothing = true; 					// 만약 아이템 이미 있으면 아무 일도 없었음		
					for (int i = 0; i < 3; ++i) {
						if (!items[i])
							chestnothing = false;
					}
					if (!chestnothing) {                   // 아이템 지급
						while (true) {
							int itemran = rand() % 3;
							if (!items[itemran]) {
								items[itemran] = true;
								break;
							}
						}
					}
				}
				else if (answerSel >= 0) {
					playerHp--;
				}
				// 확인 버튼 띄우기 (answerSel이 0이상이면 PAINT에서 띄우게 처리하면 됨.)
			}
			else {
				if (PtInRect(&eventcheck, m)) { // 확인 버튼 클릭 확인
					nextSel = answerSel = -1;
					MoveturnCnt = 10;
					start.x = 0;
					page = 1; // 확인 버튼 클릭 시, 이동 페이지로 전환
				}
			}
		}
		else {

		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
	{
		m = { LOWORD(lParam), HIWORD(lParam) };
		if ((page == 2 || page == 5) && (battleSel == 0 || battleSel == 1)) {
			for (int i = 0; i < line * line; ++i) {
				if (m.x > numblock[i].rpt.x && m.x < numblock[i].rpt.x + numblock[i].size && m.y > numblock[i].rpt.y && m.y < numblock[i].rpt.y + numblock[i].size) {
					if (numblock[i].type == 0) {
						numblock[i].type = 2;
					}
					else if (numblock[i].type == 3) {
						numblock[i].type = 1;
					}
				}
				else {
					if (numblock[i].type == 2) {
						numblock[i].type = 0;
					}
					else if (numblock[i].type == 1) {
						numblock[i].type = 3;
					}
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hWnd, iMsg, wParam, lParam));
}

void CALLBACK TimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
	gametime++;
	if (playerMove) playerMove--;

	if (playerMove && playerMove % 10 == 0) start.x += 3;

	if (playerAttack > 0) {
		playerAttack--;
		if (!playerAttack) {
			if (attackCheck) {
				if (page == 2) {
					if (battleSel != 3)
						playeratk = 30;
					battleSel = 3;
				}
				else { // 보스 상대 시,
					mob.hp--;
					if (mob.hp == 0) {

					}
					else if (mob.hp == 2) {
						bossPhase = 3;

					}
					else if (mob.hp == 4) {
						bossPhase = 2;

					}
					else {
						bossPattern = rand() % 101 + bossPatternStack;
						if (bossPattern >= 75) {
							bossPattern = 2;
							bossPatternStack = 0;
						}
						else {
							bossPattern = 0;
							bossPatternStack += 50;
							battleSel = -1;
							MoveturnCnt = 10;
						}
					}
				}
			}
			else {
				mobAttack = 50;
				playerHp--;
			}
		}
	}

	if (playerDefend > 0) {
		playerDefend--;
		if (!playerDefend) {
			mobAttack = 50;
			if (!defendCheck) {
				playerHp--;
			}
		}
	}

	if (mobAttack) {
		mobAttack--;
		if (!mobAttack) {
			if (mob.atk < 2) {
				battleSel = 4;
			}
			else {
				if (page == 2) {
					battleSel = -1;
					MoveturnCnt = 10;
				}
				else {
					bossPattern = rand() % 101 + bossPatternStack;
					if (bossPattern >= 75) {
						bossPattern = 2;
						bossPatternStack = 0;
					}
					else {
						bossPattern = 0;
						bossPatternStack += 50;
						battleSel = -1;
						MoveturnCnt = 10;
					}
				}
			}
		}
	}

	if (bossPattern) {
		if (bossPattern == 2) {
			curtain = { board.pt.x - 20, -boardsize };
			bossPattern = 1;
		}
		else if (bossPattern == 1) {
			curtain.y += 10;
			if (curtain.y == 0) {
				for (int i = 0; i < bossPhase; ++i)
					_2048blockchange();

				bossPattern = -1;
			}
		}
		else if (bossPattern == -1) {
			curtain.y -= 10;
			if (curtain.y == -boardsize) {
				bossPattern = 0;
				battleSel = -1;
				MoveturnCnt = 10;
			}
		}
	}

	if (_2048move) {
		bool    finchceck = true;
		POINT temp = { 0,0 };
		for (int k = 0; k < 7; ++k) {
			if (_2048arrow == _LT) temp = { k,0 };
			else if (_2048arrow == _RT) temp = { 6 - k,0 };
			else if (_2048arrow == _UP) temp = { 0,k };
			else if (_2048arrow == _DN) temp = { 0,6 - k };
			for (int i = 0; i < line * line; ++i) {
				if (temp.x == 0) {
					if (temp.y - 1 == numblock[i].bpt.y) {
						for (int j = 0; j < 10; ++j) {
							_2048movingblock(numblock[i]);
							if (numblock[i].dpt.x == 0 && numblock[i].dpt.y == 0) {
								_2048startingmovecheck(numblock[i]);
								for (int k = 0; k < line * line; ++k) {
									for (int l = 0; l < k; ++l) {
										if (numblock[l].rpt.x == numblock[k].rpt.x && numblock[l].rpt.y == numblock[k].rpt.y && numblock[l].num == numblock[k].num && numblock[l].num != 0) {
											numblock[l].num *= 2;
											numblock[k].bpt = { -1,-1 };
											numblock[k].dpt = { 0,0 };
											numblock[k].num = 0;
											numblock[k].rpt = { 0,0 };
											numblock[k].type = 0;
											blockcount--;
										}
									}
								}
							}
							if (numblock[i].dpt.x != 0 || numblock[i].dpt.y != 0) finchceck = false;
						}
					}
				}
				else if (temp.y == 0) {
					if (temp.x - 1 == numblock[i].bpt.x) {
						for (int j = 0; j < 10; ++j) {
							_2048movingblock(numblock[i]);
							if (numblock[i].dpt.x == 0 && numblock[i].dpt.y == 0) {
								_2048startingmovecheck(numblock[i]);
								for (int k = 0; k < line * line; ++k) {
									for (int l = 0; l < k; ++l) {
										if (numblock[l].rpt.x == numblock[k].rpt.x && numblock[l].rpt.y == numblock[k].rpt.y && numblock[l].num == numblock[k].num && numblock[l].num != 0) {
											numblock[l].num *= 2;
											numblock[k].bpt = { -1,-1 };
											numblock[k].dpt = { 0,0 };
											numblock[k].num = 0;
											numblock[k].rpt = { 0,0 };
											numblock[k].type = 0;
											blockcount--;
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
			_2048move = false;
			_2048arrow = 0;
			MoveturnCnt--;
			for (int i = 0; i < line * line; ++i) {
				if (numblock[i].type == 1) {
					numblock[i].type = 0;
				}
				else if (numblock[i].type == 2) {
					numblock[i].type = 0;
				}
				else if (numblock[i].type == 3) {
					numblock[i].type = 0;
				}
			}
			if (blockcount == line * line - 1) {
				gameover = true;
			}
			_2048createblock();
		}
	}

	InvalidateRect(hWnd, NULL, FALSE);
}