
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

    hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1100, NULL, (HMENU)NULL, hInstance, NULL);	//������ ����� �Լ�

    ShowWindow(hWnd, nCMdShow);	//	������ ����
    UpdateWindow(hWnd);		// ������ ������Ʈ

    while (GetMessage(&Message, 0, 0, 0)) {		//	�޽��� ���� (�޽��� �޾ƿ��� ����) -  ����޽����� ���ö����� �ݺ�
        TranslateMessage(&Message);		//	�޽��� ����
        DispatchMessage(&Message);	//	�޽��� ó��
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
Cell        cell[tablecount][tablecount];   // ��
Shape       shape[shapecount];              // ����
POINT       ptLT;                           // �巹���� ���� ����
POINT       ptRB;                           // �巹���� ���� ��
COLORREF    currentcolor;                   // ���� ��
int         currentshape;                   // ���� ���� ����
bool        act = false;                    // �巹��������
bool        solidpen = true;                // �Ǽ�����
COLORREF    colors[6] = { RGB(255,0,0),RGB(0,255,0), RGB(0,0,255), RGB(255,255,0), RGB(255,0,255), RGB(0,255,255) };
bool        border = false;                 // ���� �׵θ��׸���
bool        move = false;                   // ������ ���콺�� �����ϰ���
bool        inform = false;                 // ���� �������

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

// �� �׸���
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
    case WM_PAINT:  //  ��ȿȭ ������ �߻��ؾ� �Ѵ� = �ٽ� �׷����ϴ� ������ �߻��Ѱ��
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
