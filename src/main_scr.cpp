#include <Windows.h>
#include <Windowsx.h>
#include <winuser.h>
// OpenGL Includes
#include <gl\GL.h>
#include <gl\GLU.h>
// Screensaver Includes
#include <scrnsave.h>
#include <commdlg.h>
#include <CommCtrl.h>
#include <math.h>

#include "main.h"
//#include "progdef.h"
#include "resource.h"
#include "version.h"

//#include <iostream>
//using namespace std;

//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")
//#pragma comment(lib, "scrnsave.lib")
//#pragma comment(lib, "ComCtl32.lib")

extern HINSTANCE    hMainInstance;      // screen saver instance handle
extern BOOL         fChildPreview;

int width;
int height;
float koef;
static int cx;

void Render(HDC*);
void Update();
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

TScreen mntrs[10];
TBall ball;

void draw_symbol(unsigned int m, int x, int y, int cel, int row, char *p_matrix)
{
    int bm_x = 0;
    int bm_y = 0;
    int counter;

    char *p_2char = &p_matrix[cel * row * m];
    char *p_bm = p_2char;

    if(!keydown) {
        int r = rand()%1027;
        if(r < 26 && m != 8) {
           grey = r * 0.01f;
           glColor3f(grey, grey, grey);
        }
    }

    while (1)
    {
        counter = 0;
        do
        {
            if (p_bm[counter] != 48)
            {
                float ix = (x + bm_x - xoffset) * space;
                float iy = (y + bm_y - yoffset) * -space;
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

void clear_bg(HDC hDC)
{
    int ax = x_offset+21;
    int ay = y_offset-10;

    for (int iy=ay; iy < ay+22; iy++)
    {
        for (int ix=-ax; ix < -ax+43; ix++)
        {
            int r = rand()%200;
            if(r <= 5 ) {
                grey = (float)r  * 0.01f;
                glColor3f(grey, grey, grey);
                TBall_Init(&ball, ix*space, iy*space);
                TBall_Draw(ball);
            }
        }
    }
    wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
    g_start_flag = 1;
}

void ScreenSaverInit(HDC hDC)
{
    for (int iy=-cy; iy < cy; iy++)
    {
        for (int ix=-cx; ix < cx; ix++)
        {
            grey = rand()%10  * 0.01f ;
            glColor3f(grey, grey, grey);
            TBall_Init(&ball, ix*space, iy*space);
            TBall_Draw(ball);
        }
    }
    wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
}
void draw_clock(HDC *hDC)
{
    glColor3f(0.05f, 0.05f, 0.05f);

    if (keydown || st.wSecond % 2 == 0 || g_start_flag)
    {
        keydown = TRUE;
        draw_symbol(8, -19, -9, 5, 9, digit_matrix);
        draw_symbol(8, -13, -9, 5, 9, digit_matrix);
        draw_symbol(8, -5, -9, 5, 9,  digit_matrix);
        draw_symbol(8, 1, -9, 5, 9,   digit_matrix);

        draw_symbol(st.wDayOfWeek, -13, 2, 7, 5,    w_deys_matrix);
        draw_symbol(8, -4, 2, 4, 7,      m_deys_matrix);
        draw_symbol(8, 2, 2, 4, 7,       m_deys_matrix);
        keydown = FALSE;
    }
    draw_symbol(8, 9, -9, 5, 9,       digit_matrix);
    draw_symbol(8, 15, -9, 5, 9,      digit_matrix);

    draw_symbol(2, -7, -9, 1, 9,      dots_matrix);
    draw_symbol(2, 7, -9, 1, 9,       dots_matrix);

    xoffset = x_offset;
    yoffset = y_offset;

    glColor3f(0.17f, 0.15f, 0.15f);

    if (st.wSecond % 2 == 0 || g_start_flag)
    {
        g_start_flag = FALSE;
        draw_symbol(st.wHour / 10, -19, -9, 5, 9,   digit_matrix);
        draw_symbol(st.wHour % 10, -13, -9, 5, 9,   digit_matrix);
        draw_symbol(st.wMinute / 10, -5, -9, 5, 9,  digit_matrix);
        draw_symbol(st.wMinute % 10, 1, -9, 5, 9,   digit_matrix);

        glColor3f(0.15f, 0.14f, 0.14f);
        draw_symbol(st.wDayOfWeek, -13, 2, 7, 5,    w_deys_matrix);
        draw_symbol(st.wDay / 10, -4, 2, 4, 7,      m_deys_matrix);
        draw_symbol(st.wDay % 10, 2, 2, 4, 7,       m_deys_matrix);
    }

    draw_symbol(st.wSecond / 10, 9, -9, 5, 9,       digit_matrix);
    draw_symbol(st.wSecond % 10, 15, -9, 5, 9,      digit_matrix);

    int dc = st.wSecond % 2;
    draw_symbol(dc, -7, -9, 1, 9,   dots_matrix);
    draw_symbol(dc, 7, -9, 1, 9,    dots_matrix);


    if (st.wSecond % 2 == 0 || g_start_flag)
    {
        glColor3f(0.15f, 0.15f, 0.0f);
        draw_symbol(start_time.wHour / 10, -19, 11, 4, 7,   m_deys_matrix);
        draw_symbol(start_time.wHour % 10, -13, 11, 4, 7,   m_deys_matrix);
        draw_symbol(start_time.wMinute / 10, -5, 11, 4, 7,  m_deys_matrix);
        draw_symbol(start_time.wMinute % 10, 1, 11, 4, 7,   m_deys_matrix);
        draw_symbol(start_time.wSecond / 10, 9, 11, 4, 7,       m_deys_matrix);
        draw_symbol(start_time.wSecond % 10, 15, 11, 4, 7,      m_deys_matrix);
        draw_symbol(1, -7, 10, 1, 9,   dots_matrix);
        draw_symbol(1, 7, 10, 1, 9,    dots_matrix);

    }

}

void Render(HDC *hDC)
{
    /* OpenGL animation code goes here */
    for(int gw = 0; gw < 10; gw++)
    {
        float rc = rand()%20 * 0.01;
        float gc = rand()%10 * 0.01;
        glColor3f(rc, gc, 0.0f);
        float ix = rand_range(-cx, cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, (float)ix, (float)iy);
        TBall_Draw(ball);
    }
    //Sleep(10);

    GetLocalTime(&st);
    draw_clock(hDC);
    if(p_second != st.wSecond){
        clear_bg(*hDC);
        p_second = st.wSecond;
    }
    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
    Sleep(160);


    grey = rand()%10 * 0.01f;
    glColor3f(grey, grey, grey);
    for(int gw = 0; gw < 5; gw++)
    {
        float ix = rand_range(-cx, cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, ix, iy);
        TBall_Draw(ball);
        //Sleep(10);
    }
}

LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    HGLRC hRC;
    //HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
    static MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hmon, &mi);
    hDC = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);

    m_num = GetSystemMetrics(SM_CMONITORS);

    static UINT uTimer = 0;

    switch (uMsg)
    {
        case WM_CREATE:
        {
            if (MULTIMONITOR){
                width   = rc.right;
                height  = rc.bottom;
            } else {
                width   = mi.rcMonitor.right;
                height  = mi.rcMonitor.bottom;
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW);
            }

            EnableOpenGL(hwnd, &hDC, &hRC);

            koef = width > height ? (float)width / height : (float)height / width;

            cx = cy * koef;
            glLoadIdentity();
            glOrtho(-XYSCALE, XYSCALE, -XYSCALE, XYSCALE, -1, 1);
            glScalef(1/koef, 1, 1);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            setVertex();

            ScreenSaverInit(hDC);
            GetLocalTime(&start_time);

            uTimer = (UINT)SetTimer(hwnd, 1, 1, NULL);
        }

        case WM_ERASEBKGND:
        {
            // Erase screensaver background
        }
        break;

        case WM_TIMER:
        {
            // Handle timer
            Update();
        }
        break;

        case WM_DESTROY:
        {
            KillTimer(hwnd, uTimer);
            DisableOpenGL(hwnd, hDC, hRC);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
        }
        break;

        case WM_PAINT:
        {
            Render(&hDC);
        }
        break;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (!mouse_x_init)
            {
                mouse_x_init = x;
                mouse_y_init = y;
            }
            if (mouse_x_init != x || mouse_y_init != y)
            {
                PostQuitMessage(0);
            }
        }
        break;

        case WM_KEYDOWN:
        {
            switch (wParam) {
                case VK_UP:     {y_offset++;keydown=TRUE;}break;
                case VK_DOWN:   {y_offset--;keydown=TRUE;}break;
                case VK_LEFT:   {x_offset++;keydown=TRUE;}break;
                case VK_RIGHT:  {x_offset--;keydown=TRUE;}break;
                case VK_BACK:   {clear_bg(hDC);}break;
                case VK_SCROLL: break;
                default: {PostQuitMessage(0);}
            }
        }
        break;

        case WM_KEYUP:
        {
            keydown=FALSE;
            clear_bg(hDC);
        }
        break;

        default:
        {
            return DefScreenSaverProc(hwnd, uMsg, wParam, lParam);
        }
        break;
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

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RECT         rc;

    GetClientRect(hDlg, &rc);

    switch (message)
    {
    case WM_INITDIALOG:

        SetDlgItemTextA(hDlg, IDC_YEAR , AutoVersion::YEAR);
        SetDlgItemTextA(hDlg, IDC_VERION , AutoVersion::FULLVERSION_STRING);
        break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_GOGITBTN:
            ShellExecute(hwnd, "Open", GITHUBURL, (LPCTSTR)NULL, (LPCTSTR)NULL, SW_SHOW);
            break;
        case IDOK:
            EndDialog(hDlg, LOWORD(wParam));
            break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
        return TRUE;
    }
    return (FALSE);
}


BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

void Update()
{
    //Sleep(1);
}
