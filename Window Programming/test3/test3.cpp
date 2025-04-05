#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <ctime>
#include <vector>

#define MAX_LINES 10
#define MAX_CHARS_PER_LINE 30

HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Memo Notepad";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// 전역 변수들
TCHAR str[MAX_LINES][MAX_CHARS_PER_LINE + 1] = { L"\0" }; // 10줄, 각 줄에 30자까지
int currentLine = 0; // 현재 입력 중인 줄
int currentPos = 0;  // 현재 입력 중인 줄의 위치 (0~29)
bool overwriteMode = true; // 덮어쓰기 모드, 기본값 true (덮어쓰기)
bool isUpperCase = false; // 대소문자 토글 상태
std::vector<bool> hasNumber(MAX_LINES, false); // 각 줄에 숫자가 포함되었는지 여부
std::vector<bool> spaceRemoved(MAX_LINES, false); // 공백 제거 상태
std::vector<bool> replacedWithAt(MAX_LINES, false); // '@'로 교체된 문자의 줄
std::vector<bool> uppercasedWords(MAX_LINES, false); // 괄호로 감싼 대문자 상태

// 캐럿 초기화 함수
void ResetCaret() {
	currentPos = 0;
}

// 문자 삽입 함수
void InsertChar(TCHAR ch) {
	if (currentPos < MAX_CHARS_PER_LINE) {
		str[currentLine][currentPos] = ch;
		currentPos++;
	}
	else if (currentLine < MAX_LINES - 1) {
		// 한 줄이 가득 찬 경우, 다음 줄로 이동
		currentLine++;
		currentPos = 0;
		str[currentLine][currentPos] = ch;
		currentPos++;
	}
	else {
		// 마지막 줄까지 다 차면 첫 번째 줄로 돌아가서 덮어쓰기
		currentLine = 0;
		currentPos = 0;
		str[currentLine][currentPos] = ch;
		currentPos++;
	}
}

// 백스페이스 처리 함수
void HandleBackspace() {
	if (currentPos > 0) {
		// 현재 줄에서 문자를 삭제
		str[currentLine][--currentPos] = L'\0';
	}
	else if (currentLine > 0) {
		// 첫 줄이 아니면, 이전 줄로 캐럿 이동
		currentLine--;
		currentPos = MAX_CHARS_PER_LINE;
		str[currentLine][currentPos] = L'\0';
	}
}

// 엔터키 처리 함수
void HandleEnter() {
	if (currentLine < MAX_LINES - 1) {
		currentLine++;
		currentPos = 0;
	}
	else {
		// 마지막 줄까지 입력이 끝나면 첫 줄로 돌아가서 덮어쓰기
		currentLine = 0;
		currentPos = 0;
	}
	ResetCaret();
}

// Esc 키 처리 (화면 지우기)
void HandleEsc() {
	memset(str, 0, sizeof(str)); // 메모장 내용 초기화
	currentLine = 0;
	currentPos = 0;
}

// Tab 키 처리 (5개의 스페이스 삽입)
void HandleTab() {
	for (int i = 0; i < 5; i++) {
		InsertChar(L' ');
	}
}

// Home 키 처리 (줄의 맨 앞으로 이동)
void HandleHome() {
	currentPos = 0;
}

// End 키 처리 (줄의 맨 뒤로 이동)
void HandleEnd() {
	currentPos = MAX_CHARS_PER_LINE;
}

// Insert 키 처리 (삽입 모드와 덮어쓰기 모드 토글)
void HandleInsert() {
	overwriteMode = !overwriteMode; // 덮어쓰기 모드 토글
}

// Del 키 처리 (현재 단어 삭제)
void HandleDel() {
	// 현재 캐럿 위치에서 단어 삭제
	while (currentPos < MAX_CHARS_PER_LINE && str[currentLine][currentPos] != L' ' && str[currentLine][currentPos] != L'\0') {
		str[currentLine][currentPos++] = L'\0';
	}
}

// 방향키 처리
void HandleArrowKey(int direction) {
	if (direction == VK_LEFT && currentPos > 0) {
		// 왼쪽 화살표
		currentPos--;
	}
	else if (direction == VK_RIGHT && currentPos < MAX_CHARS_PER_LINE && str[currentLine][currentPos] != L'\0') {
		// 오른쪽 화살표
		currentPos++;
	}
	else if (direction == VK_UP && currentLine > 0) {
		// 위쪽 화살표
		currentLine--;
	}
	else if (direction == VK_DOWN && currentLine < MAX_LINES - 1) {
		// 아래쪽 화살표
		currentLine++;
	}
}

// 페이지업/페이지다운 처리
void HandlePageUpDown(int direction) {
	if (direction == VK_PRIOR && currentLine > 2) {
		currentLine -= 3;
	}
	else if (direction == VK_NEXT && currentLine < MAX_LINES - 3) {
		currentLine += 3;
	}
}

// F1 키: 대문자/소문자 토글
void HandleF1() {
	isUpperCase = !isUpperCase;
}

// F2 키: 숫자가 포함된 줄에 '****' 추가/삭제
void HandleF2() {
	bool hasNum = false;
	for (int i = 0; i < MAX_CHARS_PER_LINE; i++) {
		if (str[currentLine][i] >= L'0' && str[currentLine][i] <= L'9') {
			hasNum = true;
			break;
		}
	}
	if (hasNum) {
		if (str[currentLine][0] != L'*') {
			for (int i = MAX_CHARS_PER_LINE - 1; i > 3; i--) {
				str[currentLine][i] = str[currentLine][i - 4];
			}
			str[currentLine][0] = L'*';
			str[currentLine][1] = L'*';
			str[currentLine][2] = L'*';
			str[currentLine][3] = L'*';
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			str[currentLine][i] = L'\0';
		}
		for (int i = 4; i < MAX_CHARS_PER_LINE; i++) {
			if (str[currentLine][i] == L'*') {
				str[currentLine][i] = L'\0';
			}
		}
	}
}

// F3 키: 괄호로 감싸고 대문자로 출력
void HandleF3() {
	if (uppercasedWords[currentLine]) {
		// 괄호 제거 후 원래대로 복구
		uppercasedWords[currentLine] = false;
		// 원래 단어로 복원하는 코드 작성 필요
	}
	else {
		// 괄호 추가 및 대문자화
		uppercasedWords[currentLine] = true;
		for (int i = 0; i < MAX_CHARS_PER_LINE; i++) {
			if (str[currentLine][i] != L' ') {
				str[currentLine][i] = towupper(str[currentLine][i]);
			}
		}
	}
}

// F4 키: 공백 문자 삭제/복구
void HandleF4() {
	if (spaceRemoved[currentLine]) {
		// 공백 복구
		spaceRemoved[currentLine] = false;
	}
	else {
		// 공백 삭제
		for (int i = 0; i < MAX_CHARS_PER_LINE; i++) {
			if (str[currentLine][i] == L' ') {
				str[currentLine][i] = L'\0';
			}
		}
		spaceRemoved[currentLine] = true;
	}
}

// F5 키: 랜덤 문자 @로 바꾸기/복구
void HandleF5() {
	srand(time(NULL));
	int index = rand() % MAX_CHARS_PER_LINE;
	replacedWithAt[currentLine] = !replacedWithAt[currentLine];
	if (replacedWithAt[currentLine]) {
		str[currentLine][index] = L'@';
	}
	else {
		str[currentLine][index] = L'\0'; // 원래 문자로 복구
	}
}

// F6 키: 줄 순서 이동
void HandleF6() {
	if (currentLine > 0) {
		// 현재 줄의 데이터를 이전 줄로 이동
		std::swap(str[currentLine], str[currentLine - 1]);
		currentLine--;
	}
	else if (currentLine == 0 && MAX_LINES > 1) {
		// 첫 번째 줄일 때는 마지막 줄로 순환
		std::swap(str[currentLine], str[MAX_LINES - 1]);
	}
}

// + 키: 알파벳 및 숫자 모두 다음 문자로 변경
void HandlePlus() {
	for (int i = 0; i < MAX_LINES; i++) {
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++) {
			if (str[i][j] >= L'a' && str[i][j] <= L'z') {
				str[i][j] = (str[i][j] == L'z') ? L'a' : str[i][j] + 1;
			}
			else if (str[i][j] >= L'A' && str[i][j] <= L'Z') {
				str[i][j] = (str[i][j] == L'Z') ? L'A' : str[i][j] + 1;
			}
			else if (str[i][j] >= L'0' && str[i][j] <= L'9') {
				str[i][j] = (str[i][j] == L'9') ? L'0' : str[i][j] + 1;
			}
		}
	}
}

// - 키: 알파벳 및 숫자 모두 이전 문자로 변경
void HandleMinus() {
	for (int i = 0; i < MAX_LINES; i++) {
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++) {
			if (str[i][j] >= L'a' && str[i][j] <= L'z') {
				str[i][j] = (str[i][j] == L'a') ? L'z' : str[i][j] - 1;
			}
			else if (str[i][j] >= L'A' && str[i][j] <= L'Z') {
				str[i][j] = (str[i][j] == L'A') ? L'Z' : str[i][j] - 1;
			}
			else if (str[i][j] >= L'0' && str[i][j] <= L'9') {
				str[i][j] = (str[i][j] == L'0') ? L'9' : str[i][j] - 1;
			}
		}
	}
}



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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	SIZE size;

	switch (iMessage) {
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_F1) {
			HandleF1();
		}
		else if (wParam == VK_F2) {
			HandleF2();
		}
		else if (wParam == VK_F3) {
			HandleF3();
		}
		else if (wParam == VK_F4) {
			HandleF4();
		}
		else if (wParam == VK_F5) {
			HandleF5();
		}
		else if (wParam == VK_F6) {
			HandleF6();
		}
		else if (wParam == VK_ADD) {
			HandlePlus();
		}
		else if (wParam == VK_SUBTRACT) {
			HandleMinus();
		}
		else if (wParam == VK_BACK) {
			HandleBackspace();
		}
		else if (wParam == VK_ESCAPE) {
			HandleEsc();
		}
		else if (wParam == VK_RETURN) {
			HandleEnter();
		}
		else if (wParam == VK_TAB) {
			HandleTab();
		}
		else if (wParam == VK_HOME) {
			HandleHome();
		}
		else if (wParam == VK_END) {
			HandleEnd();
		}
		else if (wParam == VK_INSERT) {
			HandleInsert();
		}
		else if (wParam == VK_DELETE) {
			HandleDel();
		}
		else if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {
			HandleArrowKey(wParam);
		}
		else if (wParam == VK_PRIOR || wParam == VK_NEXT) {
			HandlePageUpDown(wParam);
		}
		else if (wParam >= 0x20 && wParam <= 0x7E) {
			// 일반 문자 처리 (A-Z, a-z, 0-9, 특수문자)
			InsertChar((TCHAR)wParam);
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetTextExtentPoint32(hDC, str[0], MAX_CHARS_PER_LINE, &size);

		for (int i = 0; i < MAX_LINES; i++) {
			TextOut(hDC, 0, 20 * i, str[i], _tcslen(str[i]));
		}

		SetCaretPos(size.cx, 20 * currentLine);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}