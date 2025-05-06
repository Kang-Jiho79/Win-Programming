#include <windows.h>
#include <math.h>

#define TIMER_ID 1
#define PI 3.14159265

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

enum Mode { NONE, UPDOWN, ROTATE };
Mode currentMode = NONE;

RECT player = { 290, 210, 310, 230 }; // 플레이어 사각형
int obstacleY[4] = { 100, 150, 200, 250 }; // 위아래 이동
int dir[4] = { 1, 1, 1, 1 };

float angle = 0.0f;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("HardestGame");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("세상에서 가장 어려운 게임 - WinAPI"),
        WS_OVERLAPPEDWINDOW, 100, 100, 600, 500, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetTimer(hwnd, TIMER_ID, 30, NULL); // 타이머 30ms 주기

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void DrawPlayer(HDC hdc) {
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    FillRect(hdc, &player, redBrush);
    DeleteObject(redBrush);
}

void DrawUpDownStage(HDC hdc) {
    DrawPlayer(hdc);
    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));

    for (int i = 0; i < 4; ++i) {
        Ellipse(hdc, 100, obstacleY[i], 120, obstacleY[i] + 20);
    }

    DeleteObject(blueBrush);
}

void DrawRotatingStage(HDC hdc) {
    DrawPlayer(hdc);
    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));

    int centerX = 200, centerY = 200;
    int radius = 80;

    for (int i = 0; i < 4; ++i) {
        float a = angle + i * (PI / 2);
        int x = centerX + (int)(cos(a) * radius);
        int y = centerY + (int)(sin(a) * radius);
        Ellipse(hdc, x - 10, y - 10, x + 10, y + 10);
    }

    DeleteObject(blueBrush);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == '1') currentMode = UPDOWN;
        else if (wParam == '2') currentMode = ROTATE;
        break;

    case WM_TIMER:
        if (currentMode == UPDOWN) {
            for (int i = 0; i < 4; ++i) {
                obstacleY[i] += dir[i] * 4;
                if (obstacleY[i] < 50 || obstacleY[i] > 350) dir[i] *= -1;
            }
        }
        else if (currentMode == ROTATE) {
            angle += 0.05f;
            if (angle > 2 * PI) angle -= 2 * PI;
        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (currentMode == UPDOWN)
            DrawUpDownStage(hdc);
        else if (currentMode == ROTATE)
            DrawRotatingStage(hdc);
        else
            TextOut(hdc, 100, 100, TEXT("1: 위아래 장애물 | 2: 회전 장애물"), 28);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}