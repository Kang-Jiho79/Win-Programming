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

#define max_line 10
#define max_char 30

TCHAR str[max_line][max_char+1] = {L"\0"};		// 전체 저장
TCHAR strout[max_line][max_char + 5] = { L"\0" };	// 출력할것
TCHAR CaretPoint[max_char + 1] = L"\0";		// 현재 캐럿위치
int count[max_line] = { 0 };				// 줄마다 글자수
int entercount = 0;					// 현재 몇번째 줄인지
int present = 0;					// 현재 몇번째 글자인지
bool insertcheck = false;			// insert 확인
int uppercheck = 0;					// F1 확인
bool starcheck = false;				// F2 확인
bool parenthesischeck = false;		// F3 확인
bool blankdeletecheck = false;		// F4 확인
bool randomcharcheck = false;		// F5 확인

// 캐럿 위치 맨앞으로(home)
void CarePointReset()
{
	for (int i = 0; i < lstrlen(CaretPoint); i++)
		CaretPoint[i] = '\0';
	present = 0;
}	   

// 캐럿 위치 세팅
void CarePointSet(int setenter, int setcount)
{
	CarePointReset();
	for (int i = 0; i < setcount; i++) {
		CaretPoint[i] = str[setenter][i];
	}
	CaretPoint[setcount] = '\0';
	present = setcount;
}   

// 문자열 저장 (insert, F1)
void insertword(TCHAR ch)
{
	if (uppercheck == 1)
		if (isalpha(ch))
			toupper(ch);
	if (uppercheck == 2)
		if (isalpha(ch))
			tolower(ch);
	if (insertcheck && count[entercount] < max_char) {
		for (int i = count[entercount]; i > present; i--)
			str[entercount][i + 1] = str[entercount][i];
	}
	else {

	}
}
// 지우기 (backspace)

// 개행 (enter)

// 문자열 출력 (F2, F3, F4, F5)

// 전체 초기화 (esc)

// 공백 5개 삽입 (tab)
void tabkey()
{
	if (count[entercount] + 5 <= 30) {
		for (int i = 0; i < 5; i++) {
			str[entercount][present] = ' ';
			CaretPoint[present++] = ' ';
			count[entercount]++;
		}
		CaretPoint[present] = '\0';
	}
	else {
		for (int i = present; i < 30; i++) {
			str[entercount][i] = ' ';
		}
		entercount++;
		CarePointSet(entercount, 0);
	}
}

// 맨 뒤로 가기(end)
void endkey()
{
	if (count[entercount] == 30) {
		entercount++;
		CarePointReset();
	}
	else {
		CarePointSet(entercount, count[entercount]);
	}
}
// 단어 지우기 (delete)
void DeleteWord()
{
	int Start = present, End = present, Endcheck = 0;
	bool Startcheck = true;
	if (str[entercount][present] != ' ') {
		while (Startcheck || Endcheck == 0) {
			if (str[entercount][Start] == ' ')
				Startcheck = false;
			else
				Start--;
			if (str[entercount][End] == ' ')
				Endcheck = 1;
			else if (str[entercount][End] == '\0')
				Endcheck = 2;
			else
				End++;
		}
		if (Endcheck == 1) {
			int s = Start;
			for (int i = End; i < count[entercount]; i++)
				str[entercount][s++] = str[entercount][i];
			StrSet(entercount, present);
			count[entercount] = count[entercount] - (End - Start);

			for (int i = count[entercount]; i < 30; i++)
				str[entercount][i] = '\0';
		}
		else {
			for (int i = Start; i < count[entercount]; i++) {
				str[entercount][i] = '\0';
			}
			count[entercount] = Start;
			StrSet(entercount, Start - 1);
		}
	}
}
// 상하좌우로 이동

// 위로 3칸이동 (Pgup)

// 밑으로 3칸이동 (Pgdn)

// 줄 쉬프트 (F6)

// 다음 문자 (+)

// 이전 문자 (-)

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	SIZE size;
	switch (iMessage) {
	case WM_CREATE:
	{
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
	}
	break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE) {		//초기화

		}
		else if (wParam == VK_TAB) {		//공백 5개 넣기 30자 넘어가면 개행
			tabkey();
		}
		else if (wParam == VK_HOME) {	//줄 맨앞으로 이동
			CarePointReset();
		}
		else if (wParam == VK_END) {	//줄 맨뒤로 이동 30자가 다 차있으면 다음줄로
			endkey();
		}
		else if (wParam == VK_INSERT) {	//사이에 문자 추가 다시누르면 덮어쓰기
			if (insertcheck)
				insertcheck = false;
			else
				insertcheck = true;
		}
		else if (wParam == VK_DELETE) { //현재 단어를 지운다
			DeleteWord();
		}
		else if (wParam == VK_LEFT) {	//캐럿 위치이동
			if (present > 0)
				CaretPoint[present--] = '\0';
		}
		else if (wParam == VK_RIGHT) {
			if (present < count[entercount])
				CaretPoint[present++] = str[entercount][present];
		}
		else if (wParam == VK_UP) {
			if (entercount > 0) {
				if (present > count[entercount - 1]) {
					StrSet(str, CaretPoint, entercount - 1, count[entercount - 1]);
					entercount--;
				}
				else
					StrSet(str, CaretPoint, --entercount, present);
			}
		}
		else if (wParam == VK_DOWN) {
			if (entercount < 9)
				if (present > count[entercount + 1]) {
					StrSet(str, CaretPoint, entercount + 1, count[entercount + 1]);
					entercount++;
				}
				else
					StrSet(str, CaretPoint, ++entercount, present);
		}
		else if (wParam == VK_PRIOR) {	//캐럿의 위치를 위/아래로 3줄 이동

		}
		else if (wParam == VK_NEXT) {

		}
		else if (wParam == VK_F1) { // 입력하는 문자가 대문자로 출력된다 다시 누르면 소문자로 출력된다

		}
		else if (wParam == VK_F2) { //숫자가 포함되어 있는 줄의 맨앞에 4칸의 *을 넣는다 다시 누르면 없어진다

		}
		else if (wParam == VK_F3) { //공백으로 구분되는 단어를 괄호가 둘러싸고 대문자로 출력한다 다시누르면 돌아온다

		}
		else if (wParam == VK_F4) { //공백문자를 삭제한다 다시 누르면 공백이 생긴다.

		}
		else if (wParam == VK_F5) {	// 랜덤한 한 문자를 선택하여 @바꾸어 출력한다

		}
		else if (wParam == VK_F6) { // 줄의 순서가 한줄씩 앞으로 이동한다.

		}
		else if (wParam == VK_BACK) {
			if (present == 0) {
				if (entercount > 0) {
					StrSet(str, CaretPoint, entercount - 1, count[entercount - 1]);
					while (str[entercount][0] != '\0' && count[entercount - 1] < 30) {
						str[entercount - 1][count[entercount - 1]++] = str[entercount][0];
						for (int i = 0; i < count[entercount]; i++)
							str[entercount][i] = str[entercount][i + 1];
						count[entercount]--;
					}
					entercount--;
				}
			}
			else {
				for (int i = present - 1; i < count[entercount] + 1; i++) {
					str[entercount][i] = str[entercount][i + 1];
				}
				count[entercount]--;
				CaretPoint[present--] = '\0';
			}
		}
		else if (wParam == VK_RETURN) {
			if (entercount == 9) {
				entercount = 0;
			}
			else {
				entercount++;
			}
			StrReset(CaretPoint);
		}
		else {

		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_CHAR:
	{
		if (wParam == VK_BACK) {
			if (present == 0) {
				if (entercount > 0) {
					StrSet(str, CaretPoint, entercount - 1, count[entercount - 1]);
					while (str[entercount][0] != '\0' && count[entercount - 1] < 30) {
						str[entercount - 1][count[entercount - 1]++] = str[entercount][0];
						for (int i = 0; i < count[entercount]; i++)
							str[entercount][i] = str[entercount][i + 1];
						count[entercount]--;
					}
					entercount--;
				}
			}
			else {
				for (int i = present - 1; i < count[entercount] + 1; i++) {
					str[entercount][i] = str[entercount][i + 1];
				}
				count[entercount]--;
				CaretPoint[present--] = '\0';
			}
		}
		
		else if (wParam != VK_TAB) {
			if (insertcheck) {
				if (count[entercount] <= 29) {
					if (present < count[entercount]) {
						for (int i = count[entercount]; i > present - 1; i--) {
							str[entercount][i + 1] = str[entercount][i];
						}
						str[entercount][present] = wParam;
						CaretPoint[present++] = wParam;
						count[entercount]++;
					}
					else {
						str[entercount][count[entercount]++] = wParam;
						CaretPoint[present++] = wParam;
					}

				}
				else if (count[entercount] == 30) {
					if (entercount == 9) {
						entercount = 0;
					}
					else {
						entercount++;
					}
					StrReset(CaretPoint);
					if (present < count[entercount]) {
						for (int i = count[entercount]; i > present; i--) {
							str[entercount][i + 1] = str[entercount][i];
						}
						str[entercount][present] = wParam;
						CaretPoint[present++] = wParam;
						count[entercount]++;
					}
					else {
						str[entercount][count[entercount]++] = wParam;
						CaretPoint[present++] = wParam;
					}
				}
			}
			else {
				if (count[entercount] <= 29) {
					if (present < count[entercount]) {
						str[entercount][present] = wParam;
						CaretPoint[present++] = wParam;
						count[entercount]++;
					}
					else {
						str[entercount][count[entercount]++] = wParam;
						CaretPoint[present++] = wParam;
					}
				}
				else if (count[entercount] == 30) {
					if (entercount == 9) {
						entercount = 0;
					}
					else {
						entercount++;
					}
					StrReset(CaretPoint);
					if (present < count[entercount]) {
						str[entercount][present] = wParam;
						CaretPoint[present++] = wParam;
						count[entercount]++;
					}
					else {
						str[entercount][count[entercount]++] = wParam;
						CaretPoint[present++] = wParam;
					}
				}
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetTextExtentPoint32(hDC, CaretPoint, present, &size);
		for (int i = 0; i < 10; i++)
			TextOut(hDC, 0, 20 * i, str[i], _tcsclen(str[i]));
		SetCaretPos(size.cx, 20 * entercount);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}