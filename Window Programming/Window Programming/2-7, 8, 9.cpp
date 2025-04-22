//�ǽ� 7, 8, 9 �޸���
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR lpszCmdParam, int nCMdShow)	// �����Լ� 
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;	//������ Ŭ����
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);	//	����ü�� ũ��
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	//	������ ��� ��Ÿ��
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	//	������ �޼����� ó���ϴ� ������ ���ν��� �Լ� �̸� (Long Pointer Funtion WndProc)
	WndClass.cbClsExtra = 0;	//	��뿩�� �޸�(class)
	WndClass.cbWndExtra = 0;	//	��뿩�� �޸�(Window)
	WndClass.hInstance = hInstance;		//���� ���α׷� ID
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// ���������� ���� ����������
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		// �����쿡�� ���� Ŀ������
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// �������� ������ �����Ҽ��ִ�.
	WndClass.lpszMenuName = NULL;	//�޴� �̸�
	WndClass.lpszClassName = IpszClass;		//	Ŭ������ �̸�
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//���� ������ (���� hIcon�� ������ ���)
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);	//������ ����� �Լ�

	ShowWindow(hWnd, nCMdShow);	//	������ ����
	UpdateWindow(hWnd);		// ������ ������Ʈ

	while (GetMessage(&Message, 0, 0, 0)) {		//	�޽��� ���� (�޽��� �޾ƿ��� ����) -  ����޽����� ���ö����� �ݺ�
		TranslateMessage(&Message);		//	�޽��� ����
		DispatchMessage(&Message);	//	�޽��� ó��
	}
	return Message.wParam;
}

#define max_line 10
#define max_char 30

TCHAR str[max_line][max_char + 1] = { L"\0" };		// ��ü ����
std::wstring strout[max_line] = { L"\0" };	// ����Ұ�
TCHAR CaretPoint[max_char + 1] = L"\0";		// ���� ĳ����ġ
int count[max_line] = { 0 };			// �ٸ��� ���ڼ�
int entercount = 0;					// ���� ���° ������
int present = 0;					// ���� ���° ��������
bool insertcheck = false;			// insert Ȯ��
int uppercheck = 0;					// F1 Ȯ��
bool starcheck = false;				// F2 Ȯ��
bool parenthesischeck = false;		// F3 Ȯ��
bool spacedeletecheck = false;		// F4 Ȯ��
bool randomcharcheck = false;		// F5 Ȯ��



// ĳ�� ��ġ �Ǿ�����(home)
void CaretPointReset()
{
	for (int i = 0; i < lstrlen(CaretPoint); i++)
		CaretPoint[i] = '\0';
	present = 0;
}

// ĳ�� ��ġ ����
void CaretPointSet(int setenter, int setcount)
{
	CaretPointReset();
	for (int i = 0; i < setcount; i++) {
		CaretPoint[i] = str[setenter][i];
	}
	CaretPoint[setcount] = '\0';
	present = setcount;
}

// ���� (enter)
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

// ���ڿ� ���� (insert, F1)
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

// ����� (backspace)
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

// �빮�� ��ȯ
std::wstring toUpper(std::wstring& word) {
	std::wstring result;
	for (TCHAR c : word) {
		result += std::towupper(c);
	}
	return result;
}
// .����
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

// ���ڿ� ��� (F2, F3, F4, F5)
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

// ��ü �ʱ�ȭ (esc)
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

// ���� 5�� ���� (tab)
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

// �� �ڷ� ����(end)
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

// �ܾ� ����� (delete)
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

// �����¿�� �̵�
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

// ���Ʒ��� 3ĭ�̵� (Pgup / Pgdn)
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

// �� ����Ʈ (F6)
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


// ���� ���� (+)
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


// ���� ���� (-)
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
			if (wParam == VK_ESCAPE) {		//�ʱ�ȭ
				esckey();
			}
			else if (wParam == VK_TAB) {		//���� 5�� �ֱ� 30�� �Ѿ�� ����
				tabkey();
			}
			else if (wParam == VK_HOME) {	//�� �Ǿ����� �̵�
				CaretPointReset();
			}
			else if (wParam == VK_END) {	//�� �ǵڷ� �̵� 30�ڰ� �� �������� �����ٷ�
				endkey();
			}
			else if (wParam == VK_INSERT) {	//���̿� ���� �߰� �ٽô����� �����
				insertcheck = !insertcheck;
			}
			else if (wParam == VK_DELETE) { //���� �ܾ �����
				DeleteWord();
			}
			else if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {	//ĳ�� ��ġ�̵�
				CaretPointmove(wParam);
			}
			else if (wParam == VK_PRIOR || wParam == VK_NEXT) {	//ĳ���� ��ġ�� ��/�Ʒ��� 3�� �̵�
				threetimemove(wParam);
			}
			else if (wParam == VK_F1) { // �Է��ϴ� ���ڰ� �빮�ڷ� ��µȴ� �ٽ� ������ �ҹ��ڷ� ��µȴ�
				if (uppercheck == 2)
					uppercheck = 0;
				else
					uppercheck++;
			}
			else if (wParam == VK_F6) { // ���� ������ ���پ� ������ �̵��Ѵ�.
				F6key();
			}
			else if (wParam == VK_BACK) {
				backspacekey();
			}
			else if (wParam == VK_RETURN) {
				enterkey();
			}
		}
		if (wParam == VK_F2 && !parenthesischeck && !spacedeletecheck && !randomcharcheck) { //���ڰ� ���ԵǾ� �ִ� ���� �Ǿտ� 4ĭ�� *�� �ִ´� �ٽ� ������ ��������
			starcheck = !starcheck;
		}
		else if (wParam == VK_F3 && !starcheck && !spacedeletecheck && !randomcharcheck) { //�������� ���еǴ� �ܾ ��ȣ�� �ѷ��ΰ� �빮�ڷ� ����Ѵ� �ٽô����� ���ƿ´�
			parenthesischeck = !parenthesischeck;
		}
		else if (wParam == VK_F4 && !starcheck && !parenthesischeck && !randomcharcheck) { //���鹮�ڸ� �����Ѵ� �ٽ� ������ ������ �����.
			spacedeletecheck = !spacedeletecheck;
		}
		else if (wParam == VK_F5 && !starcheck && !parenthesischeck && !spacedeletecheck) {	// ������ �� ���ڸ� �����Ͽ� @�ٲپ� ����Ѵ�
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