//실습 7, 8, 9 메모장
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <string>
#include <iostream>
#include <cwctype>

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

TCHAR str[max_line][max_char + 1] = { L"\0" };		// 전체 저장
std::wstring strout[max_line] = { L"\0" };	// 출력할것
TCHAR CaretPoint[max_char + 1] = L"\0";		// 현재 캐럿위치
int count[max_line] = { 0 };			// 줄마다 글자수
int entercount = 0;					// 현재 몇번째 줄인지
int present = 0;					// 현재 몇번째 글자인지
bool insertcheck = false;			// insert 확인
int uppercheck = 0;					// F1 확인
bool starcheck = false;				// F2 확인
bool parenthesischeck = false;		// F3 확인
bool spacedeletecheck = false;		// F4 확인
bool randomcharcheck = false;		// F5 확인



// 캐럿 위치 맨앞으로(home)
void CaretPointReset()
{
	for (int i = 0; i < lstrlen(CaretPoint); i++)
		CaretPoint[i] = '\0';
	present = 0;
}

// 캐럿 위치 세팅
void CaretPointSet(int setenter, int setcount)
{
	CaretPointReset();
	for (int i = 0; i < setcount; i++) {
		CaretPoint[i] = str[setenter][i];
	}
	CaretPoint[setcount] = '\0';
	present = setcount;
}

// 개행 (enter)
void enterkey()
{
	if (entercount == 9) {
		entercount = 0;
	}
	else {
		entercount++;
	}
	CaretPointReset();
}

// 문자열 저장 (insert, F1)
void insertword(TCHAR ch)
{
	TCHAR c;
	if (uppercheck == 1) {
		if (isalpha(ch))
			c = towupper(ch);
	}
	else if (uppercheck == 2) {
		if (isalpha(ch))
			c = towlower(ch);
	}
	else
		c = ch;
	if (insertcheck&& count[entercount] < max_char) {
		for (int i = count[entercount]; i > present; i--)
			str[entercount][i + 1] = str[entercount][i];
		str[entercount][present] = c;
		CaretPoint[present++] = c;
		count[entercount]++;
	}
	else if (count[entercount] < max_char) {
		if (present < count[entercount]) {
			str[entercount][present] = c;
			CaretPoint[present++] = c;
		}
		else {
			str[entercount][present] = c;
			CaretPoint[present++] = c;
			count[entercount]++;
		}
	}
	else if (!insertcheck) {
		enterkey();
		if (present < count[entercount]) {
			str[entercount][present] = c;
			CaretPoint[present++] = c;
		}
		else {
			str[entercount][present] = c;
			CaretPoint[present++] = c;
			count[entercount]++;
		}
	}
}

// 지우기 (backspace)
void backspacekey()
{
	if (present > 0) {
		for (int i = present - 1; i < count[entercount]; i++) {
			str[entercount][i] = str[entercount][i + 1];
		}
		count[entercount]--;
		CaretPoint[present--] = '\0';
	}
	else if (present == 0 && entercount > 0) {
		entercount--;
		CaretPointSet(entercount, count[entercount]);
	}
}

// 대문자 변환
std::wstring toUpper(std::wstring& word) {
	std::wstring result;
	for (TCHAR c : word) {
		result += std::towupper(c);
	}
	return result;
}
// .구분
std::wstring strdot(std::wstring& word, std::wstring& dot) {
	std::wstring clean;
	dot.clear();
	for (TCHAR c : word) {
		if (iswalnum(c)) {
			clean += c;
		}
		else {
			dot += c;
		}
	}
	return clean;
}

// 문자열 출력 (F2, F3, F4, F5)
void strset()
{

	for (int i = 0; i < max_line; i++) {
		strout[i].clear();
		if (starcheck) {
			bool numbercheck = false;
			for (int j = 0; j < count[i]; j++)
				if (iswdigit(str[i][j]))
					numbercheck = true;
			if (numbercheck)
				strout[i] += std::wstring(L"****") + str[i];
			else
				strout[i] += str[i];
		}
		else if (parenthesischeck) {
			std::wstringstream ss(str[i]);
			std::wstring w;
			while (ss >> w) {
				std::wstring dot;
				std::wstring word = strdot(w, dot);
				strout[i] += L"(" + toUpper(word) + L")" + dot;
			}
		}
		else if (spacedeletecheck) {
			for (int j = 0; j < count[i]; j++)
				if (!iswspace(str[i][j]))
					strout[i] += str[i][j];
		}
		else if (randomcharcheck) {
			std::srand(std::time(0));
			TCHAR ch = 'a' + std::rand() % 26;;
			for (int j = 0; j < count[i]; j++)
				if (str[i][j] == ch || str[i][j] == towupper(ch))
					strout[i] += '@';
				else
					strout[i] += str[i][j];
		}
		else {
			for (int j = 0; j < count[i]; j++)
				strout[i] += str[i][j];
		}
	}
}

// 전체 초기화 (esc)
void esckey()
{
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 31; j++) {
			str[i][j] = '\0';
			CaretPoint[j] = '\0';
		}
		count[i] = 0;
	}
	entercount = 0;
	present = 0;
}

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
		CaretPointSet(entercount, 0);
	}
}

// 맨 뒤로 가기(end)
void endkey()
{
	if (count[entercount] == 30) {
		entercount++;
		CaretPointReset();
	}
	else {
		CaretPointSet(entercount, count[entercount]);
	}
}

// 단어 지우기 (delete)
void DeleteWord()
{
	int len = count[entercount], start = present, end = present;

	while (start > 0 && str[entercount][start - 1] != ' ') {
		start--;
	}

	while (end < len && str[entercount][end] != ' ') {
		end++;
	}
	int dellen = end - start;

	for (int i = end; i < len; i++) {
		str[entercount][i - dellen] = str[entercount][i];
	}
	for (int i = len - dellen; i < len; i++) {
		str[entercount][i] = '\0';
	}
	count[entercount] -= dellen;

	if (start >= count[entercount]) {
		int newpos = count[entercount] - 1;
		while (newpos > 0 && str[entercount][newpos] == ' ')newpos--;
		CaretPointSet(entercount, newpos >= 0 ? newpos : 0);
	}
	else
		CaretPointSet(entercount, start);
}

// 상하좌우로 이동
void CaretPointmove(TCHAR ch)
{
	if (ch == VK_LEFT) {
		if (present > 0)
			CaretPoint[present--] = '\0';
	}
	else if (ch == VK_RIGHT) {
		if (present < count[entercount])
			CaretPoint[present++] = str[entercount][present];
	}
	else if (ch == VK_UP) {
		if (entercount > 0) {
			entercount--;
			if (present > count[entercount])
				CaretPointSet(entercount, count[entercount]);
			else
				CaretPointSet(entercount, present);
		}
	}
	else {
		if (entercount < 9) {
			entercount++;
			if (present > count[entercount])
				CaretPointSet(entercount, count[entercount]);
			else
				CaretPointSet(entercount, present);
		}
	}
}

// 위아래로 3칸이동 (Pgup / Pgdn)
void threetimemove(TCHAR ch)
{
	if (ch == VK_PRIOR) {
		if (entercount > 2) {
			if (present > count[entercount - 3]) {
				entercount = entercount - 3;
				CaretPointSet(entercount, count[entercount]);
			}
			else {
				entercount = entercount - 3;
				CaretPointSet(entercount, present);
			}
		}
		else {
			if (present > count[0]) {
				entercount = 0;
				CaretPointSet(entercount, count[entercount]);
			}
			else {
				entercount = 0;
				CaretPointSet(entercount, present);
			}
		}
	}
	else {
		if (entercount < 7) {
			if (present > count[entercount + 3]) {
				entercount = entercount + 3;
				CaretPointSet(entercount, count[entercount]);
			}
			else {
				entercount = entercount + 3;
				CaretPointSet(entercount, present);
			}
		}
		else {
			if (present > count[9]) {
				entercount = 9;
				CaretPointSet(entercount, count[entercount]);
			}
			else {
				entercount = 9;
				CaretPointSet(entercount, present);
			}
		}
	}
}

// 줄 쉬프트 (F6)
void F6key()
{
	int first, check = 0, firstcount;
	TCHAR temp[31];
	for (int i = max_line - 1; i > -1; i--) {
		if (str[i][0] >= 0x20 && str[i][0] <= 0x7E) {
			if (check != 0) {

				for (int j = i + 1; j < max_line; j++)
					if (str[j][0] >= 0x20 && str[j][0] <= 0x7E) {
						for (int x = 0; x < max_char; x++)
							str[j][x] = str[i][x];
						count[j] = count[i];
						break;
					}
			}
			else {
				for (int x = 0; x < max_char; x++)
					temp[x] = str[i][x];
				firstcount = count[i];
			}
			check++;
		}
	}
	for (int i = 0; i < max_line; i++)
		if (str[i][0] >= 0x20 && str[i][0] <= 0x7E) {
			first = i;
			break;
		}
	for (int x = 0; x < max_char; x++)
		str[first][x] = temp[x];
	count[first] = firstcount;
	CaretPointReset();
}


// 다음 문자 (+)
void charplus()
{
	for (int i = 0; i < max_line; i++) {
		for (int j = 0; j < max_char; j++) {
			if (str[i][j] >= 'a' && str[i][j] <= 'z') {
				str[i][j] = (str[i][j] == 'z') ? 'a' : str[i][j] + 1;
			}
			else if (str[i][j] >= 'A' && str[i][j] <= 'Z') {
				str[i][j] = (str[i][j] == 'Z') ? 'A' : str[i][j] + 1;
			}
			else if (str[i][j] >= L'0' && str[i][j] <= L'9') {
				str[i][j] = (str[i][j] == '9') ? '0' : str[i][j] + 1;
			}
		}
	}
}


// 이전 문자 (-)
void charminus()
{
	for (int i = 0; i < max_line; i++) {
		for (int j = 0; j < max_char; j++) {
			if (str[i][j] >= 'a' && str[i][j] <= 'z') {
				str[i][j] = (str[i][j] == 'a') ? 'z' : str[i][j] - 1;
			}
			else if (str[i][j] >= 'A' && str[i][j] <= 'Z') {
				str[i][j] = (str[i][j] == 'A') ? 'Z' : str[i][j] - 1;
			}
			else if (str[i][j] >= L'0' && str[i][j] <= L'9') {
				str[i][j] = (str[i][j] == '0') ? '9' : str[i][j] - 1;
			}
		}
	}
}


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
		if (!starcheck && !parenthesischeck && !spacedeletecheck && !randomcharcheck) {
			if (wParam == VK_ESCAPE) {		//초기화
				esckey();
			}
			else if (wParam == VK_TAB) {		//공백 5개 넣기 30자 넘어가면 개행
				tabkey();
			}
			else if (wParam == VK_HOME) {	//줄 맨앞으로 이동
				CaretPointReset();
			}
			else if (wParam == VK_END) {	//줄 맨뒤로 이동 30자가 다 차있으면 다음줄로
				endkey();
			}
			else if (wParam == VK_INSERT) {	//사이에 문자 추가 다시누르면 덮어쓰기
				insertcheck = !insertcheck;
			}
			else if (wParam == VK_DELETE) { //현재 단어를 지운다
				DeleteWord();
			}
			else if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {	//캐럿 위치이동
				CaretPointmove(wParam);
			}
			else if (wParam == VK_PRIOR || wParam == VK_NEXT) {	//캐럿의 위치를 위/아래로 3줄 이동
				threetimemove(wParam);
			}
			else if (wParam == VK_F1) { // 입력하는 문자가 대문자로 출력된다 다시 누르면 소문자로 출력된다
				if (uppercheck == 2)
					uppercheck = 0;
				else
					uppercheck++;
			}
			else if (wParam == VK_F6) { // 줄의 순서가 한줄씩 앞으로 이동한다.
				F6key();
			}
			else if (wParam == VK_BACK) {
				backspacekey();
			}
			else if (wParam == VK_RETURN) {
				enterkey();
			}
		}
		if (wParam == VK_F2 && !parenthesischeck && !spacedeletecheck && !randomcharcheck) { //숫자가 포함되어 있는 줄의 맨앞에 4칸의 *을 넣는다 다시 누르면 없어진다
			starcheck = !starcheck;
		}
		else if (wParam == VK_F3 && !starcheck && !spacedeletecheck && !randomcharcheck) { //공백으로 구분되는 단어를 괄호가 둘러싸고 대문자로 출력한다 다시누르면 돌아온다
			parenthesischeck = !parenthesischeck;
		}
		else if (wParam == VK_F4 && !starcheck && !parenthesischeck && !randomcharcheck) { //공백문자를 삭제한다 다시 누르면 공백이 생긴다.
			spacedeletecheck = !spacedeletecheck;
		}
		else if (wParam == VK_F5 && !starcheck && !parenthesischeck && !spacedeletecheck) {	// 랜덤한 한 문자를 선택하여 @바꾸어 출력한다
			randomcharcheck = !randomcharcheck;
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;

	case WM_CHAR:
	{
		if (!starcheck && !parenthesischeck && !spacedeletecheck && !randomcharcheck) {
			if (wParam == '+') {
				charplus();
			}
			else if (wParam == '-') {
				charminus();
			}
			else if (wParam >= 0x20 && wParam <= 0x7E) {
				insertword(wParam);
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		strset();
		GetTextExtentPoint32(hDC, CaretPoint, present, &size);
		for (int i = 0; i < 10; i++)
			TextOut(hDC, 0, 20 * i, strout[i].c_str(), strout[i].length());
		SetCaretPos(size.cx, 20 * entercount);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}