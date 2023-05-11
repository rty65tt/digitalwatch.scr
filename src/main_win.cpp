#include <windows.h>
#include <Windowsx.h>
#include <winuser.h>

#include <gl/gl.h>
#include <math.h>
//#include <scrnsave.h>

#include <CommCtrl.h>
#include "CommDlg.h"
#include "main.h"

//#include <vector>
#include <iostream>
using namespace std;
//#include "resource.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void drawCircleFill(int cnt)
{
    float x, y;
    float a = M_PI * 2.0 / cnt;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= cnt; i++)
    {
        x = sin(a * i);
        y = cos(a * i);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawSquareFill()
{
    float x=1;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-x, x);
    glVertex2f(x, x);
    glVertex2f(x, -x);
    glVertex2f(-x, -x);
    glEnd();
}

typedef struct
{
    BOOL flag_init = TRUE;
    HWND hwnd;
    int left,right,top,buttom;
    float koef;
    int cx;
} TScreen;

TScreen *mons_screen;

typedef struct
{
    float x,y;
    float r;
} TBall;

TBall ball;

void TBall_Init(TBall *obj, float x1, float y1, float r1)
{
    obj->x = x1;
    obj->y = y1;
    obj->r = r1;
}

void TBall_Draw(TBall obj)
{
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0);
    glScalef(obj.r, obj.r, 1);
    //drawSquareFill();
    drawCircleFill(CIRCLE_EDGES);
    glPopMatrix();
}

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
                TBall_Init(&ball, ix, iy, step);
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

void Screensaver_Init(HDC* hDC, int mon_num)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);


    for (int iy=-cy; iy <= cy; iy++)
    {
        for (int ix=-mons_screen[mon_num].cx; ix <= mons_screen[mon_num].cx; ix++)
        {
            float grey = rand()%10  * 0.01f ;
            glColor3f(0.1f + grey, grey, 0.0f);
            TBall_Init(&ball, (float)ix * space, (float)iy * space, step);
            TBall_Draw(ball);
        }
    }
    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
}


float rand_range(int range_min, int range_max)
{
    int r = ((double)rand() / RAND_MAX) * (range_max - range_min) + range_min;
    return r;
}

void draw_clock(HDC* hDC, int mon_num)
{
    if(p_second != st.wSecond || mons_screen[mon_num].flag_init)
    {   // background font
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

        wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
        //Sleep(10);
    }

    int c_count = (st.wSecond == 0) ? 20 : g_start_flag;

    // background grey circles
    if(!c_count)
    {
        float grey = rand()%10 * 0.01f;
        glColor3f(grey, grey, grey);
        float ix = rand_range(-mons_screen[mon_num].cx, mons_screen[mon_num].cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, (float)ix, (float)iy, step);
        TBall_Draw(ball);
        Sleep(100);
    } else {
        // color circles
        float rc = rand()%10 * 0.01;
        float gc = rand()%20 * 0.01;
        glColor3f(0.15+rc, gc, 0.0f);
        float ix = rand_range(-mons_screen[mon_num].cx, mons_screen[mon_num].cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, (float)ix, (float)iy, step);
        TBall_Draw(ball);
    }
    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
    g_start_flag = 0;
}

BOOL CALLBACK MyInfoEnumProc(
  HMONITOR hMonitor,	// handle to display monitor
  HDC hdcMonitor,   	// handle to monitor DC
  LPRECT lprcMonitor,   // monitor intersection rectangle
  LPARAM dwData     	// data
)
{
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;

    if(mons_screen[mon_count].flag_init) {

        cout << "=== MonitorEnumProc START ==== Mon # "<< mon_count << " / " << m_num << " ============\n";

        static MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        HWND desktop = GetDesktopWindow();
        RECT s_size;
        GetWindowRect(desktop, &s_size);
        GetMonitorInfo(hMonitor, &mi);
        int width = mi.rcMonitor.right - mi.rcMonitor.left;
        int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

//        cout << "=== GetMonitorInfo(hMonitor, &mi); ====" << endl;
//        cout << "Monitor NUM = " << mon_count << endl;
//        cout << "Resolution = " << width << " x " << height << endl;
//        cout << "left = " << mi.rcMonitor.left << " top = " << mi.rcMonitor.top << endl;
//        cout << "right = " << mi.rcMonitor.right << " bottom = " << mi.rcMonitor.bottom << endl;

        mons_screen[mon_count].koef = width > height ? (float)width / height: (float)height / width;
        mons_screen[mon_count].cx = cy * mons_screen[mon_count].koef;

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
        sprintf(class_name, "WindowClass%d", mon_count);
        wcex.lpszClassName = class_name;
        wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

        if (!RegisterClassEx(&wcex))
            return 0;

        /* create main window */
        hwnd = CreateWindowEx(NULL,
                              class_name,
                              class_name,
                              //WS_OVERLAPPEDWINDOW,
                              WS_POPUP | WS_VISIBLE,
                              mi.rcMonitor.left,      //CW_USEDEFAULT,
                              0,      //CW_USEDEFAULT,
                              width,
                              height,
                              NULL,
                              NULL,
                              hMainInstance,
                              NULL);

        ShowWindow(hwnd, SW_SHOW);

        /* enable OpenGL for the window */
        mons_screen[mon_count].hwnd = hwnd;
        hwnd = mons_screen[mon_count].hwnd;
        EnableOpenGL(hwnd, &hDC, &hRC);
        glLoadIdentity();
        glOrtho(-XYSCALE, XYSCALE, -XYSCALE, XYSCALE, -1, 1);
        glScalef( 1 / mons_screen[mon_count].koef, 1, 1);
        Screensaver_Init(&hDC, mon_count);
        //DisableOpenGL(hwnd, hDC, hRC);        mons_screen[mon_count].flag_init = FALSE;
//    cout << "=== MonitorEnumProc END = Mon # " << mon_count << " ============\n" << endl;
    }
    hwnd = mons_screen[mon_count].hwnd;
    hDC = GetDC(hwnd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
//    EnableOpenGL(hwnd, &hDC, &hRC);
    glLoadIdentity();
    glOrtho(-XYSCALE, XYSCALE, -XYSCALE, XYSCALE, -1, 1);
    glScalef( 1 / mons_screen[mon_count].koef, 1, 1);
    draw_clock(&hDC, mon_count);
    DisableOpenGL(hwnd, hDC, hRC);
    mon_count++;
    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    hMainInstance = hInstance;
    HWND hwnd;
    MSG msg;
    BOOL bQuit = FALSE;

    m_num = GetSystemMetrics(SM_CMONITORS);

    TScreen mons[m_num];
    mons_screen = mons;

    EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

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
            mon_count = 0;
            EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);
            p_second = st.wSecond;
            //Sleep(400);
        }
    }

    /* shutdown OpenGL */
    //DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        ShowCursor(FALSE);
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
        PostQuitMessage(0);
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

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
