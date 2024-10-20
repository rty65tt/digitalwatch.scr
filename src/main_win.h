#include "main.h"

HINSTANCE    hMainInstance;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);

void ScreenSaverInit(HDC*, int);
void set_scale(int);
void render(int);

TScreen *mntrs;
TBall ball;


BOOL CALLBACK MonEnumProcInit(
    HMONITOR hMonitor,	// handle to display monitor
    HDC hdcMonitor,   	// handle to monitor DC
    LPRECT lprcMonitor,   // monitor intersection rectangle
    LPARAM dwData     	// data
)
{
    static MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HWND desktop = GetDesktopWindow();
    RECT s_size;
    GetWindowRect(desktop, &s_size);
    GetMonitorInfo(hMonitor, &mi);
    int width = mi.rcMonitor.right - mi.rcMonitor.left;
    int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    mntrs[mcnt].koef = width > height ? (float)width / height: (float)height / width;
    mntrs[mcnt].cx = cy * mntrs[mcnt].koef;
    mntrs[mcnt].width = width;
    mntrs[mcnt].height = height;
    mntrs[mcnt].left = mi.rcMonitor.left;
    mntrs[mcnt].top = mi.rcMonitor.top;
    mntrs[mcnt].right = mi.rcMonitor.right;
    mntrs[mcnt].bottom = mi.rcMonitor.bottom;

    wchar_t class_name[256];

    WNDCLASSEX wcex;
    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hMainInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    swprintf(class_name, L"DigitalWatch%d", mcnt);
    wcex.lpszClassName = class_name;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    mntrs[mcnt].hwnd = CreateWindowEx(WS_EX_TOPMOST,
                          class_name,
                          class_name,
                          //WS_OVERLAPPEDWINDOW,
                          WS_POPUP | WS_VISIBLE,
                          mi.rcMonitor.left,    //CW_USEDEFAULT,
                          0,                    //CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          hMainInstance,
                          NULL);

    ShowWindow(mntrs[mcnt].hwnd, SW_SHOW);
//    SetWindowPos(mntrs[mcnt].hwnd, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW);

    /* enable OpenGL for the window */
    EnableOpenGL(mntrs[mcnt].hwnd, &mntrs[mcnt].hDC, &mntrs[mcnt].hRC);
    wglMakeCurrent(NULL, NULL);

    mcnt++;
    return TRUE;
}

BOOL CALLBACK MonEnumRandReset(
    HMONITOR hMonitor,	// handle to display monitor
    HDC hdcMonitor,   	// handle to monitor DC
    LPRECT lprcMonitor,   // monitor intersection rectangle
    LPARAM dwData     	// data
)
{
    wglMakeCurrent(mntrs[mcnt].hDC, mntrs[mcnt].hRC);
    circle = circle_bg;
    set_scale(scl_bg);
    ScreenSaverInit(&mntrs[mcnt].hDC, mcnt);
    circle = circle_fg;

    mcnt++;
    return TRUE;
}

BOOL CALLBACK MonEnumProcLoop(
    HMONITOR hMonitor,	// handle to display monitor
    HDC hdcMonitor,   	// handle to monitor DC
    LPRECT lprcMonitor,   // monitor intersection rectangle
    LPARAM dwData     	// data
)
{
    wglMakeCurrent(mntrs[mcnt].hDC, mntrs[mcnt].hRC);

    render(mcnt);
    //wglMakeCurrent(NULL, NULL);
    mntrs[mcnt].flag_init = FALSE;
    mcnt++;
    return TRUE;
}

BOOL CALLBACK MonEnumOpenglDisable(
    HMONITOR hMonitor,	// handle to display monitor
    HDC hdcMonitor,   	// handle to monitor DC
    LPRECT lprcMonitor,   // monitor intersection rectangle
    LPARAM dwData     	// data
)
{
    HWND hwnd = mntrs[mcnt].hwnd;
    HDC hDC   = mntrs[mcnt].hDC;
    HGLRC hRC = mntrs[mcnt].hRC;

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);

    /* destroy the window explicitly */
    DestroyWindow(mntrs[mcnt].hwnd);
    mcnt++;
    return TRUE;
}




