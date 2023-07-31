#define UNICODE 1
/* Multi-Monitor Version */
#include <windows.h>
#include <Windowsx.h>

#include <gl/gl.h>

#include <math.h>

#include <stdio.h>

#include "main_win.h"
#include "resource.h"
#include "version.h"

BOOL WINAPI ScreenSaverConfigureDialog(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


BOOL rnd_grey_color(int m, int r_max=RAND_MAX)
{
    if(!keydown && m != 8 && st.wSecond !=0)
    {
        int r = rand()%r_max;
        if(r < 50)
        {
            grey = 25 + r;
            glColor3ub(grey, grey, grey);
            return FALSE;
        }
    }
    return TRUE;
}

void draw_symbol(unsigned int m, int x, int y, int cel, int row, int mon_num, char *p_matrix)
{
    int bm_x = 0;
    int bm_y = 0;
    int counter;
    char *p_2char = &p_matrix[cel * row * m];
    char *p_bm = p_2char;

    glColor3ubv(colors);
    BOOL a = rnd_grey_color(m);

    while (1)
    {
        counter = 0;
        do
        {
            if (p_bm[counter] != 48)
            {
                if(rand()%1000<2)
                {
                    break;
                }
                float ix = (x + bm_x) * space;
                float iy = (y + bm_y) * -space;
                rnd_grey_color(m);
                TBall_Init(&ball, ix, iy);
                TBall_Draw(ball);
                if(a)
                {
                    glColor3ubv(colors);
                }
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
    glColor3ubv(colors);
}

void draw_clock(int mon_num)
{

    circle = circle_cl;
    set_scale(scl_fg);
    // background font
    colors = def_bg_colors;
    draw_symbol(8, -19, -8, 5, 9,   mon_num, digit_matrix); //hh
    draw_symbol(8, -13, -8, 5, 9,   mon_num, digit_matrix); //hh
    draw_symbol(8, -5, -8, 5, 9,    mon_num, digit_matrix); //mm
    draw_symbol(8, 1, -8, 5, 9,     mon_num, digit_matrix); //mm
    draw_symbol(8, 9, -8, 5, 9,     mon_num, digit_matrix); //ss
    draw_symbol(8, 15, -8, 5, 9,    mon_num, digit_matrix); //ss
    draw_symbol(2, -7, -8, 1, 9,    mon_num, dots_matrix); //::
    draw_symbol(2, 7, -8, 1, 9,     mon_num, dots_matrix); //::

    // foreground font
    colors = def_hh_colors;
    draw_symbol(st.wHour / 10, -19, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wHour % 10, -13, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wMinute / 10, -5, -8, 5, 9,  mon_num, digit_matrix);
    draw_symbol(st.wMinute % 10, 1, -8, 5, 9,   mon_num, digit_matrix);

    // Day Of Week / Month Day
    colors = def_ww_colors;
    draw_symbol(st.wDayOfWeek, -13, 3, 7, 5,    mon_num, w_deys_matrix);
    draw_symbol(st.wDay / 10, -4, 2, 4, 7,      mon_num, m_deys_matrix);
    draw_symbol(st.wDay % 10, 2, 2, 4, 7,       mon_num, m_deys_matrix);

    // Seconds
    if (st.wSecond % 2 == 1)
    {
        colors = def_hh_colors;
    }
    draw_symbol(st.wSecond / 10, 9, -8, 5, 9,   mon_num, digit_matrix);
    draw_symbol(st.wSecond % 10, 15, -8, 5, 9,  mon_num, digit_matrix);

    //dot blink
    int dc = st.wSecond % 2;
    draw_symbol(dc, -7, -8, 1, 9,   mon_num, dots_matrix);
    draw_symbol(dc, 7, -8, 1, 9,    mon_num, dots_matrix);
    set_scale(scl_bg);
}

void set_scale(int sn)
{
    float scale = s[sn];
    glLoadIdentity();
    glOrtho(-XYSCALE*scale, XYSCALE*scale, -XYSCALE*scale, XYSCALE*scale, -1, 1);
    glScalef( 1 / mntrs[mcnt].koef, 1, 1);
}

void render(int mon_num)
{

    circle = circle_fg;

    int c_count = (st.wSecond== 0) ? 100 : g_start_flag;

    // background grey circles
    if(!c_count)
    {
        for (int i=0; i<32; i++)
        {
            float ix = rand_range(-mntrs[mon_num].cx, mntrs[mon_num].cx) * space;
            float iy = rand_range(-cy, cy) * space;
            GLubyte grey = rand()%15;
            glColor3ub(grey, grey, grey);
            TBall_Init(&ball, ix, iy);
            TBall_Draw(ball);
        }
        Sleep(sleeptime);
    }
    else
    {
        // color circles
        for (int i=0; i<c_count; i++)
        {
            GLubyte rc = rand()%25;
            glColor3ub(rc, rc, rc);
            float ix = rand_range(-mntrs[mon_num].cx, mntrs[mon_num].cx) * space;
            float iy = rand_range(-cy, cy) * space;
            TBall_Init(&ball, ix, iy);
            TBall_Draw(ball);
        }
        Sleep(16);
    }

    GetLocalTime(&st);
    draw_clock(mon_num);

    g_start_flag = 0;

}

void ScreenSaverInit(HDC* hDC, int mon_num)
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);

    circle = circle_bg;

    for (int iy=-cy; iy <= cy; iy++)
    {
        for (int ix=-mntrs[mon_num].cx; ix <= mntrs[mon_num].cx; ix++)
        {
            GLubyte grey = rand()%25  ;
            glColor3ub(grey, grey, grey);
            TBall_Init(&ball, (float)ix * space, (float)iy * space);
            TBall_Draw(ball);
        }
    }
}

void reset()
{
    mcnt = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnumRandReset, 0);
}

void loop()
{
    mcnt = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnumProcLoop, 0);
}

void CreateWins()
{
    mcnt = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnumProcInit, 0);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    hMainInstance = hInstance;
    MSG msg;
    BOOL bQuit = FALSE;
    HWND hwnd;

    m_num = GetSystemMetrics(SM_CMONITORS);

    TScreen mons[m_num];
    mntrs = mons;
    sleeptime = sleeptime / m_num;

    LoadSaveSettings(FALSE);
    CreateWins();
    reset();

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
            loop();
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
        switch (wParam)
        {
        case VK_INSERT:
        {
            scl_fg = ++scl_fg < 3 ? scl_fg : 0;
            reset();
        }
        break;
        case VK_DELETE:
        {
            circle_cl = circle_cl ? FALSE : TRUE;
            reset();
        }
        break;
        case VK_HOME:
        {} break;
        case VK_END:
        {
            circle_fg = circle_fg ? FALSE : TRUE;
        }
        break;
        case VK_PRIOR:
        {
            scl_bg = ++scl_bg < 3 ? scl_bg : 0;
            reset();
        }
        break;
        case VK_NEXT:
        {
            circle_bg = circle_bg ? FALSE : TRUE;
            reset();
        }
        break;
        case VK_BACK:
        {
            scl_fg      =   0;
            scl_bg      =    0;
            circle_cl   =   FALSE;
            circle_fg   =   FALSE;
            circle_bg   =   FALSE;
            reset();
        }
        break;
        case VK_F1:
        {
            ShellExecute(hwnd, L"Open", GITHUBURL, (LPCTSTR)NULL, (LPCTSTR)NULL, SW_SHOW);
            PostQuitMessage(0);
        }
        break;
        case VK_SCROLL:
            break;
        default:
        {
            PostQuitMessage(0);
        }
        }
        LoadSaveSettings(TRUE);
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

