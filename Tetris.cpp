// Tetris.cpp : Defines the entry point for the application.
//

#include "Tetris.h"

#include "framework.h"

constexpr int MAX_LOADSTRING = 255;
constexpr int MAIN_WINDOW_WIDTH = 600;
constexpr int MAIN_WINDOW_HEIGHT = 800;
constexpr int BOARD_WIDTH = 8;
constexpr int BOARD_HEIGHT = 40;
constexpr int BITMAP_SIZE = 48;

enum Block
{
    Wall,
    I,
    O,
    S,
    Z,
    L,
    J,
    T,
    None
};

// Global Variables:
HINSTANCE hInst;                                 // current instance
WCHAR szTitle[MAX_LOADSTRING] = TEXT("Tetris");  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING] =
    TEXT("Main");  // the main window class name
int score = 0;
Block** board;
std::map<int, HBITMAP> blockBitmap;
int currentBlock[4][2];

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
void InitMainWindow(HDC, int, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void PrintTetrisBoard(HDC, int width, int height);
void MoveBlock(int valueX, int valueY);
int CheckLine();
void RemoveLine(int h);
void RotateBlock(bool isClockDirection);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine, int nCmdShow)
{
    // TODO: Place code here.

    board = new Block*[BOARD_HEIGHT];
    for (int i = 0; i < BOARD_HEIGHT; ++i)
    {
        board[i] = new Block[BOARD_WIDTH];
        for (int j = 0; j < BOARD_WIDTH; ++j) board[j][i] = None;
    }

    blockBitmap[I] = LoadBitmapW(hInstance, TEXT("IDC_Blue"));
    blockBitmap[O] = LoadBitmapW(hInstance, TEXT("IDC_Yellow"));
    blockBitmap[S] = LoadBitmapW(hInstance, TEXT("IDC_Red"));
    blockBitmap[Z] = LoadBitmapW(hInstance, TEXT("IDC_Red"));
    blockBitmap[L] = LoadBitmapW(hInstance, TEXT("IDC_Green"));
    blockBitmap[J] = LoadBitmapW(hInstance, TEXT("IDC_Green"));
    blockBitmap[T] = LoadBitmapW(hInstance, TEXT("IDC_Purple"));

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable =
        LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TETRIS);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;  // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 990,
                              240, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT,
                              nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message)
    {
        case WM_CREATE:
        {
            break;
        }
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                int srcX, srcY;
                case VK_LEFT:
                {
                    MoveBlock(-1, 0);
                }
                case VK_RIGHT:
                {
                    MoveBlock(1, 0);
                }
                case VK_DOWN:
                {
                    MoveBlock(0, -1);
                }
                case VK_UP:
                {
                    RotateBlock(true);
                }
                case VK_SPACE:
                {
                }
            }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);

            RECT rect;

            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            InitMainWindow(hdc, width, height);
            PrintTetrisBoard(hdc, width, height);

            EndPaint(hWnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/// @brief 메인 윈도우 초기화
/// @param hdc : 메인 윈도우 Device Context Handle
/// @param width : 메인 클라이언트 너비
/// @param height : 메인 클라이언트 높이
void InitMainWindow(HDC hdc, int width, int height)
{
    HPEN pen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, 2, 2, nullptr);
    LineTo(hdc, width / 2, 2);
    MoveToEx(hdc, width / 2, 2, nullptr);
    LineTo(hdc, width / 2, height - 2);
    MoveToEx(hdc, width / 2, height - 2, nullptr);
    LineTo(hdc, 2, height - 2);
    MoveToEx(hdc, 2, height - 2, nullptr);
    LineTo(hdc, 2, 2);
    pen = (HPEN)SelectObject(hdc, oldPen);
    DeleteObject(pen);

    TCHAR str[1024];

    wsprintf(str, TEXT("����: %d"), score);
    TextOutW(hdc, width / 4 * 3, height / 3, str, lstrlenW(str));
}

/// @brief 테트리스 보드를 윈도우에 출력
/// @param hdc : 메인 윈도우 핸들
/// @param windowWidth : 메인 윈도우 너비
/// @param windowHeight : 메인 윈도우 높이
void PrintTetrisBoard(HDC hdc, int windowWidth, int windowHeight)
{
    const int boardEndX = windowWidth / 2 - 2;
    const int boardStartX = 2;
    const int boardEndY = 2;
    const int boardStartY = windowHeight / 2 - 2;

    const int width = boardEndX - boardEndY;
    const int height = boardEndY - boardEndY;

    for (int w = 0; w < BOARD_WIDTH; ++w)
    {
        for (int h = 0; h < BOARD_HEIGHT; ++h)
        {
            switch (board[w][h])
            {
                case Wall:
                    break;
                case None:
                    break;
                default:
                    HDC MemDC = CreateCompatibleDC(hdc);
                    HBITMAP oldBitmap = static_cast<HBITMAP>(
                        SelectObject(MemDC, blockBitmap[board[w][h]]));
                    int blockStartX = w * width / BOARD_WIDTH + boardStartX;
                    int blockStartY = h * height / BOARD_HEIGHT + boardStartY;
                    BitBlt(hdc, 0, 0, BITMAP_SIZE, BITMAP_SIZE, MemDC,
                           blockStartX, blockStartY, SRCCOPY);
                    SelectObject(MemDC, oldBitmap);
            }
        }
    }
}

/// @brief 윈래 위치에 있는 블럭을 새로운 위치로 옮깁니다.
void MoveBlock(int valueX, int valueY)
{
    for (int i = 0; i < 4; ++i)
    {
        int srcX, srcY, tgtX, tgtY;
        srcX = currentBlock[i][0];
        srcY = currentBlock[i][1];
        tgtX = srcX + valueX;
        tgtY = srcY + valueY;
        if (tgtX < 0 || tgtX >= BOARD_WIDTH || tgtY < 0 || tgtY > BOARD_HEIGHT)
            return;
        if (board[tgtX][tgtY] != None || board[srcX][srcY] == None) return;
    }

    for (int i = 0; i < 4; ++i)
    {
        int srcX, srcY, tgtX, tgtY;
        srcX = currentBlock[i][0];
        srcY = currentBlock[i][1];
        tgtX = srcX + valueX;
        tgtY = srcY + valueY;
        board[tgtX][tgtY] = board[srcX][srcY];
        board[srcX][srcY] = None;

        return;
    }
}

/// @brief 꽉찬 줄의 인덱스를 찾습니다.
/// @return 아래에서부터 찾은 첫번째 꽉찬 줄의 인덱스를 반환
int CheckLine()
{
    bool isFull = true;
    for (int i = 0; i < BOARD_HEIGHT; ++i)
    {
        for (int j = 0; j < BOARD_WIDTH; ++j)
        {
            if (board[j][i] == None) isFull = false;
        }
        if (isFull) return i;
    }

    return -1;
}

/// @brief 주어진 인자의 줄을 삭제합니다.
/// @param h : 삭제할 줄의 인덱스를 입력합니다.
void RemoveLine(int h)
{
    for (int i = h + 1; i < BOARD_HEIGHT; ++i)
    {
        for (int j = 0; j < BOARD_WIDTH; ++j) board[j][i - 1] = board[j][i];
    }
    for (int j = 0; j < BOARD_WIDTH; ++j) board[j][BOARD_HEIGHT - 1] = None;
}

void RotateBlock(bool isClockDirection) {}