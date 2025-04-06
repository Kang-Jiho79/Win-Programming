// WinAPI 기반의 턴제 격자 게임
#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <string>
#include <vector>

#define GRID_SIZE 40
#define GRID_COUNT 40
#define WINDOW_WIDTH (GRID_SIZE * GRID_COUNT)
#define WINDOW_HEIGHT (GRID_SIZE * GRID_COUNT)

enum Shape { TRIANGLE, RECTANGLE, CIRCLE, ELLIPSE };

struct Player {
    int x, y;
    COLORREF color;
    int size; // 1~GRID_SIZE
    Shape shape;
    int shapeTimer;
};

struct Cell {
    int type; // 0: 일반, 1: 장애물, 2: 색상 변경, 3: 축소, 4: 확대, 5: 모양 변경
    COLORREF color;
};

Player players[2];
int currentPlayer = 0;
Cell grid[GRID_COUNT][GRID_COUNT];

Shape goalShape;
COLORREF goalColor;
int goalSize;
POINT goalPos;

void InitGame() {
    srand((unsigned)time(0));
    for (int i = 0; i < GRID_COUNT; ++i) {
        for (int j = 0; j < GRID_COUNT; ++j) {
            grid[i][j].type = 0;
            grid[i][j].color = RGB(255, 255, 255);
        }
    }

    for (int i = 0; i < 20; ++i) {
        int x = rand() % GRID_COUNT;
        int y = rand() % GRID_COUNT;
        grid[y][x].type = 1 + rand() % 5;
        grid[y][x].color = RGB(rand() % 256, rand() % 256, rand() % 256);
    }

    players[0] = { 0, 0, RGB(rand() % 256, rand() % 256, rand() % 256), GRID_SIZE, (Shape)(rand() % 4), 0 };
    players[1] = { GRID_COUNT - 1, 0, RGB(rand() % 256, rand() % 256, rand() % 256), GRID_SIZE, (Shape)(rand() % 4), 0 };

    goalPos = { GRID_COUNT / 2, GRID_COUNT - 1 };
    goalColor = RGB(rand() % 256, rand() % 256, rand() % 256);
    goalSize = GRID_SIZE;
    goalShape = (Shape)(rand() % 4);
}

void DrawGrid(HDC hdc) {
    // 각 셀의 색상 출력
    for (int y = 0; y < GRID_COUNT; ++y) {
        for (int x = 0; x < GRID_COUNT; ++x) {
            HBRUSH brush = CreateSolidBrush(grid[y][x].color);
            RECT rect = {
                x * GRID_SIZE,
                y * GRID_SIZE,
                (x + 1) * GRID_SIZE,
                (y + 1) * GRID_SIZE
            };
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
        }
    }

    // 그리드 선
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    SelectObject(hdc, pen);
    for (int i = 0; i <= GRID_COUNT; ++i) {
        MoveToEx(hdc, i * GRID_SIZE, 0, NULL);
        LineTo(hdc, i * GRID_SIZE, WINDOW_HEIGHT);
        MoveToEx(hdc, 0, i * GRID_SIZE, NULL);
        LineTo(hdc, WINDOW_WIDTH, i * GRID_SIZE);
    }
    DeleteObject(pen);
}

void DrawShape(HDC hdc, int x, int y, COLORREF color, int size, Shape shape) {
    int cx = x * GRID_SIZE + GRID_SIZE / 2;
    int cy = y * GRID_SIZE + GRID_SIZE / 2;
    HBRUSH brush = CreateSolidBrush(color);
    SelectObject(hdc, brush);

    switch (shape) {
    case TRIANGLE:
    {
        POINT pts[3] = {
            {cx, cy - size / 2},
            {cx - size / 2, cy + size / 2},
            {cx + size / 2, cy + size / 2}
        };
        Polygon(hdc, pts, 3);
    }
    break;
    case RECTANGLE:
        Rectangle(hdc, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
        break;
    case CIRCLE:
        Ellipse(hdc, cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
        break;
    case ELLIPSE:
        Ellipse(hdc, cx - size / 2, cy - size / 3, cx + size / 2, cy + size / 3);
        break;
    }
    DeleteObject(brush);
}

void MovePlayer(int dx, int dy) {
    Player& p = players[currentPlayer];
    int nx = p.x + dx;
    int ny = p.y + dy;
    if (nx < 0 || ny < 0 || nx >= GRID_COUNT || ny >= GRID_COUNT)
        return;
    if (grid[ny][nx].type == 1)
        return;

    p.x = nx;
    p.y = ny;

    Cell& cell = grid[ny][nx];
    switch (cell.type) {
    case 2: p.color = RGB(rand() % 256, rand() % 256, rand() % 256); break;
    case 3: p.size = (p.size > GRID_SIZE / 4) ? p.size - GRID_SIZE / 4 : p.size + GRID_SIZE / 4; break;
    case 4: p.size = (p.size < GRID_SIZE) ? p.size + GRID_SIZE / 4 : p.size - GRID_SIZE / 4; break;
    case 5:
        p.shape = (Shape)(rand() % 4);
        p.shapeTimer = 5;
        break;
    }

    if (p.shapeTimer > 0) {
        --p.shapeTimer;
        if (p.shapeTimer == 0)
            p.shape = (Shape)(rand() % 4); // 되돌리는 대신 랜덤으로
    }

    if (p.x == goalPos.x && p.y == goalPos.y && p.shape == goalShape && p.color == goalColor && p.size == goalSize) {
        MessageBox(NULL, currentPlayer == 0 ? _T("플레이어 1 승리!") : _T("플레이어 2 승리!"), _T("게임 종료"), MB_OK);
    }

    currentPlayer = 1 - currentPlayer;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        InitGame();
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawGrid(hdc);

        for (int i = 0; i < 2; ++i)
            DrawShape(hdc, players[i].x, players[i].y, players[i].color, players[i].size, players[i].shape);

        DrawShape(hdc, goalPos.x, goalPos.y, goalColor, goalSize, goalShape);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
        if (wParam == 'Q') PostQuitMessage(0);
        else if (wParam == 'R') InitGame();
        else {
            if (currentPlayer == 0) {
                if (wParam == 'W') MovePlayer(0, -1);
                else if (wParam == 'A') MovePlayer(-1, 0);
                else if (wParam == 'S') MovePlayer(0, 1);
                else if (wParam == 'D') MovePlayer(1, 0);
            }
            else {
                if (wParam == 'I') MovePlayer(0, -1);
                else if (wParam == 'J') MovePlayer(-1, 0);
                else if (wParam == 'K') MovePlayer(0, 1);
                else if (wParam == 'L') MovePlayer(1, 0);
            }
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("TurnGameWindow");
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(wc.lpszClassName, _T("턴제 격자 게임"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WINDOW_WIDTH + 16, WINDOW_HEIGHT + 39, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
