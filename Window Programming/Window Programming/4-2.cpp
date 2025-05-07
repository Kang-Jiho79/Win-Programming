
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>

#define tablecount 25
#define cellsize 40
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


struct Cell
{
    POINT       pt;
    COLORREF    color;
};
struct Shape
{
    POINT       start;
    POINT       end;
    COLORREF    color;
    bool        exist;
};
Cell        cell[tablecount][tablecount];   // 셀
Shape       shape[shapecount];              // 도형
POINT       ptLT;                           // 드레그한 도형 시작
POINT       ptRB;                           // 드레그한 도형 끝
COLORREF    currentcolor;                   // 현재 색
int         currentshape;                   // 현재 선택 도형
bool        act = false;                    // 드레그중인지
bool        solidpen = true;                // 실선인지
COLORREF    colors[6] = { RGB(255,0,0),RGB(0,255,0), RGB(0,0,255), RGB(255,255,0), RGB(255,0,255), RGB(0,255,255) };
bool        border = false;                 // 도형 테두리그릴지
bool        move = false;                   // 오른쪽 마우스로 움직일건지
bool        inform = false;                 // 정보 출력할지

void settinggame()
{
    for (int y = 0; y <tablecount; ++y)
        for (int x = 0; x < tablecount; ++x) {
            cell[y][x].pt.x = x;
            cell[y][x].pt.y = y;
            cell[y][x].color = RGB(0, 0, 0);
        }
    for (int i = 0; i < shapecount; ++i)
        shape[i].exist = false;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:  //  무효화 영역이 발생해야 한다 = 다시 그려야하는 영역이 발생한경우
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HPEN hRedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

        HPEN hDefPen = (HPEN)SelectObject(hdc, hRedPen);
        HBRUSH hDefBrush = (HBRUSH)SelectObject(hdc, hBlackBrush);

        if (act) {
            Rectangle(hdc, ptLT.x, ptLT.y, ptRB.x, ptRB.y);
        }

        SelectObject(hdc, hDefPen);
        SelectObject(hdc, hDefBrush);
        DeleteObject(hRedPen);
        DeleteObject(hBlackBrush);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_LBUTTONDOWN:
    {
        ptLT.x = LOWORD(lParam);
        ptLT.y = HIWORD(lParam);
        act = true;
    }
    break;

    case WM_MOUSEMOVE:
    {
        ptRB.x = LOWORD(lParam);
        ptRB.y = HIWORD(lParam);
        InvalidateRect(hWnd, nullptr, true);
    }
    break;

    case WM_LBUTTONUP:
    {
        for (int i = 0; i < shapecount; ++i) {

        }
        act = false;
        InvalidateRect(hWnd, nullptr, true);
    }
    break;
    /*
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_UP:
            objPos.y -= 10;
            InvalidateRect(hWnd, nullptr, true);
            break;
        case VK_DOWN:
            objPos.y += 10;
            InvalidateRect(hWnd, nullptr, true);
            break;
        case VK_LEFT:
            objPos.x -= 10;
            InvalidateRect(hWnd, nullptr, true);
            break;
        case VK_RIGHT:
            objPos.x += 10;
            InvalidateRect(hWnd, nullptr, true);
        }

    }
        break;*/
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
