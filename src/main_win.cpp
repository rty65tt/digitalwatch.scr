/* Multi-Monitor Version */

#include <windows.h>
#include <Windowsx.h>
#include <winuser.h>

#include <gl/gl.h>
//#include <gl/GLU.h>
#include <math.h>
//#include <scrnsave.h>

//#include <CommCtrl.h>
//#include "CommDlg.h"

#include "main.h"
#include "resource.h"
#include "version.h"

//#include <vector>
#include <iostream>
using namespace std;
//#include "resource.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float s[3] = { 1.0f, 0.75f, 0.5f };
int scl_bg  = 0;
int scl_fg  = 0;

TScreen *mntrs;
TBall ball;

void draw_symbol(unsigned int m, int x, int y, int cel, int row, int mon_num, char *p_matrix)
{
    int bm_x = 0;
    int bm_y = 0;
    int counter;
    char *p_2char = &p_matrix[cel * row * m];
    char *p_bm = p_2char;

    while (1)
    {
        counter = 0;
        do
        {
            if (p_bm[counter] != 48)
            {
                float ix = (x + bm_x) * space;
                float iy = (y + bm_y) * -space;
                TBall_Init(&ball, ix, iy);
                TBall_Draw(ball);
            }
            bm_x += 1;
            ++counter;
        }
        while (counter < cel);
        if (!--row)
            break;
        bm_y += 1;
        p_bm += cel;
        bm_x = 0;
        counter = 0;
    }
}

void ScreenSaverInit(HDC* hDC, int mon_num)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    for (int iy=-cy; iy <= cy; iy++)
    {
        for (int ix=-mntrs[mon_num].cx; ix <= mntrs[mon_num].cx; ix++)
        {
            float grey = rand()%10  * 0.01f ;
            glColor3f(0.1f + grey, grey, 0.0f);
            TBall_Init(&ball, (float)ix * space, (float)iy * space);
            TBall_Draw(ball);
        }
    }
    //wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
}

void draw_clock(int mon_num)
{
    // background font
    glColor3f(0.05f, 0.05f, 0.05f);
    draw_symbol(8, -19, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(8, -13, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(8, -5, -8, 5, 9,    mon_num, digit_matrix);
    draw_symbol(8, 1, -8, 5, 9,     mon_num, digit_matrix);
    draw_symbol(8, 9, -8, 5, 9,     mon_num, digit_matrix);
    draw_symbol(8, 15, -8, 5, 9,    mon_num, digit_matrix);
    draw_symbol(2, -7, -8, 1, 9,    mon_num, dots_matrix);
    draw_symbol(2, 7, -8, 1, 9,     mon_num, dots_matrix);

        glColor3f(0.3f, 0.15f, 0.0f);
    draw_symbol(st.wHour / 10, -19, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wHour % 10, -13, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wMinute / 10, -5, -8, 5, 9,  mon_num, digit_matrix);
    draw_symbol(st.wMinute % 10, 1, -8, 5, 9,   mon_num, digit_matrix);

    if (st.wSecond % 2 == 1) {
        glColor3f(0.2f, 0.25f, 0.15f);
        draw_symbol(st.wDayOfWeek, -13, 3, 7, 5,    mon_num, w_deys_matrix);
        draw_symbol(st.wDay / 10, -4, 2, 4, 7,      mon_num, m_deys_matrix);
        draw_symbol(st.wDay % 10, 2, 2, 4, 7,       mon_num, m_deys_matrix);
    }
    // Seconds
    draw_symbol(st.wSecond / 10, 9, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wSecond % 10, 15, -8, 5, 9,  mon_num, digit_matrix);
    //Dot blink
    int dc = st.wSecond % 2;
    draw_symbol(dc, -7, -8, 1, 9,   mon_num, dots_matrix);
    draw_symbol(dc, 7, -8, 1, 9,    mon_num, dots_matrix);
}

void render(int mon_num)
{
    GetLocalTime(&st);
    if(mntrs[mon_num].p_second != st.wSecond){
        draw_clock(mon_num);
        //wglMakeCurrent(mntrs[mcnt].hDC, mntrs[mcnt].hRC);
        //wglSwapLayerBuffers(mntrs[mon_num].hDC, WGL_SWAP_MAIN_PLANE);
        mntrs[mon_num].p_second = st.wSecond;
    }

    int c_count = (st.wSecond == 0) ? 100 : g_start_flag;

    // background grey circles
    if(!c_count)
    {
        for (int i=0; i<32; i++){
        float grey = rand()%10 * 0.01f;
        glColor3f(grey, grey, grey);
        float ix = rand_range(-mntrs[mon_num].cx, mntrs[mon_num].cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, ix, iy);
        TBall_Draw(ball);
        }
        Sleep(sleeptime);
    } else {
        // color circles
        for (int i=0; i<c_count; i++){
            float rc = rand()%10 * 0.01;
            float gc = rand()%10 * 0.01;
            glColor3f(0.15+rc, gc, 0.0f);
            float ix = rand_range(-mntrs[mon_num].cx, mntrs[mon_num].cx) * space;
            float iy = rand_range(-cy, cy) * space;
            TBall_Init(&ball, ix, iy);
            TBall_Draw(ball);
        }
        Sleep(16);
    }
    //wglSwapLayerBuffers(mntrs[mon_num].hDC, WGL_SWAP_MAIN_PLANE);
    g_start_flag = 0;

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

BOOL CALLBACK MonEnumRandReset(
  HMONITOR hMonitor,	// handle to display monitor
  HDC hdcMonitor,   	// handle to monitor DC
  LPRECT lprcMonitor,   // monitor intersection rectangle
  LPARAM dwData     	// data
)
{
    wglMakeCurrent(mntrs[mcnt].hDC, mntrs[mcnt].hRC);
    circle = circle_bg;
    scale = s[scl_bg];
    glLoadIdentity();
    glOrtho(-XYSCALE*scale, XYSCALE*scale, -XYSCALE*scale, XYSCALE*scale, -1, 1);
    glScalef( 1 / mntrs[0].koef, 1, 1);
    ScreenSaverInit(&mntrs[mcnt].hDC, mcnt);

    circle = circle_fg;
    scale = s[scl_fg];
    glLoadIdentity();
    glOrtho(-XYSCALE*scale, XYSCALE*scale, -XYSCALE*scale, XYSCALE*scale, -1, 1);
    glScalef( 1 / mntrs[0].koef, 1, 1);
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

    char class_name[256];

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
    sprintf(class_name, "WindowClass%d", mcnt);
    wcex.lpszClassName = class_name;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    mntrs[mcnt].hwnd = CreateWindowEx(NULL,
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

    /* enable OpenGL for the window */
    EnableOpenGL(mntrs[mcnt].hwnd, &mntrs[mcnt].hDC, &mntrs[mcnt].hRC);
    glLoadIdentity();
    glOrtho(-XYSCALE, XYSCALE, -XYSCALE, XYSCALE, -1, 1);
    glScalef( 1 / mntrs[mcnt].koef, 1, 1);
    ScreenSaverInit(&mntrs[mcnt].hDC, mcnt);
    glLoadIdentity();
    glOrtho(-XYSCALE*scale, XYSCALE*scale, -XYSCALE*scale, XYSCALE*scale, -1, 1);
    glScalef( 1 / mntrs[0].koef, 1, 1);
    wglMakeCurrent(NULL, NULL);

    mcnt++;
    return TRUE;
}

void reset() {
    mcnt = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnumRandReset, 0);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    hMainInstance = hInstance;
    MSG msg;
    BOOL bQuit = FALSE;

    m_num = GetSystemMetrics(SM_CMONITORS);

    TScreen mons[m_num];
    mntrs = mons;
    sleeptime = sleeptime / m_num;

    EnumDisplayMonitors(NULL, NULL, MonEnumProcInit, 0);


    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);

                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            GetLocalTime(&st);
            mcnt = 0;
            EnumDisplayMonitors(NULL, NULL, MonEnumProcLoop, 0);

            //p_second = st.wSecond;
            //Sleep(400);
        }
    }

    /* shutdown OpenGL */
    mcnt = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnumOpenglDisable, 0);



    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static UINT uTimer = 0;

    switch (uMsg)
    {
        case WM_TIMER:
        {
            // Handle timer
            reset();
        }
        case WM_CREATE:
        {
            ShowCursor(FALSE);
            setVertex();
            //uTimer = (UINT)SetTimer(hwnd, 1, 65000, NULL);
        }
        break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            if (!mouse_x_init)
            {
                mouse_x_init = x;
            }
            if (mouse_x_init != x)
            {
                PostQuitMessage(0);
            }
        }
        break;

        case WM_KEYDOWN:
        {
            switch (wParam) {
                case VK_INSERT: {
                    circle_fg = circle_fg ? FALSE : TRUE;
                    reset();} break;
                case VK_DELETE: {
                    circle_bg = circle_bg ? FALSE : TRUE;
                    reset();} break;
                case VK_HOME:   {reset();} break;
                case VK_END:    {reset();} break;
                case VK_PRIOR:  {
                    scl_fg = ++scl_fg < 3 ? scl_fg : 0;
                    reset();    } break;
                case VK_NEXT:   {
                    scl_bg = ++scl_bg < 3 ? scl_bg : 0;
                    reset();    } break;
                case VK_SCROLL: break;
                default: {PostQuitMessage(0);}
            }
            //PostQuitMessage(0);
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_SWAP_LAYER_BUFFERS;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

