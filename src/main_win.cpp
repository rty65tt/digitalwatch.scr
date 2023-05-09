#include <windows.h>
#include <Windowsx.h>
#include <winuser.h>

#include <gl/gl.h>
#include <math.h>
#include <scrnsave.h>

#include <CommCtrl.h>
#include "CommDlg.h"

#include <iostream>
using namespace std;
//#include "SimpleIni.h"
#include "resource.h"

//#define PI       3.14159265358979323846   // pi
#define TIMER   1

extern HINSTANCE    hMainInstance;      // screen saver instance handle
extern BOOL         fChildPreview;

int width;
int height;
float koef;

static int mouse_x_init = 0;
static int mon_count = 0;

static SYSTEMTIME st;
static int g_start_flag = 400;
static int p_second;

static char digit_matrix[451] = "\
111111000110001100011000110001100011000111111\
000110000100001000010000100001000010000100001\
111110000100001000011111110000100001000011111\
111110000100001000010111100001000010000111111\
100011000110001100011111100001000010000100001\
111111000010000100001111100001000010000111111\
111111000010000100001111110001100011000111111\
111111000100001000010000100001000010000100001\
111111000110001100011111110001100011000111111\
111111000110001100011111100001000010000111111";

static char dots_matrix[136] = "\
001000100\
000101000\
001101100";

static char m_deys_matrix[281] = "1111100110011001100110011111001100010001000100010001000111110001000111111000100011111111000100010111000100011111100110011001111100010001000111111000100011110001000111111111100010001111100110011111111110010001001001000100010011111001100111111001100111111111100110011111000100011111";
static char w_deys_matrix[246] = "11101111010100111010010101001110111111010110101011010111101010110101011110111101001011100101010010111001011101111000101100011110001001110100101011110100101110010001001000100101110111101001010100101010010101001011101111000100100011110001011110111";

static float space;
static float step;
static int cx;
static int cy;

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
    int left,right,top,buttom;
    float koef,step,space;
    int cx,cy;
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
    drawCircleFill(12);
    glPopMatrix();
}

void draw_symbol(unsigned int m, int x, int y, int cel, int row, char *p_matrix)
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


void Screensaver_Init(HDC hDC)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);


    for (float iy=-cy; iy < cy; iy++)
    {
        for (float ix=-cx; ix < cx; ix++)
        {
            float grey = rand()%10  * 0.01f ;
            glColor3f(0.1f + grey, grey, 0.0f);
            TBall_Init(&ball, (float)ix*space, (float)iy*space, step);
            TBall_Draw(ball);
        }
    }
    wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
}


float rand_range(int range_min, int range_max)
{
    int r = ((double)rand() / RAND_MAX) * (range_max - range_min) + range_min;
    return r;
}

void draw_clock(HDC* hDC)
{
    if(p_second != st.wSecond || g_start_flag)
    {   // background font
        glColor3f(0.05f, 0.05f, 0.05f);
        draw_symbol(8, -19, -8, 5, 9, digit_matrix);
        draw_symbol(8, -13, -8, 5, 9, digit_matrix);
        draw_symbol(8, -5, -8, 5, 9, digit_matrix);
        draw_symbol(8, 1, -8, 5, 9, digit_matrix);
        draw_symbol(8, 9, -8, 5, 9, digit_matrix);
        draw_symbol(8, 15, -8, 5, 9, digit_matrix);
        draw_symbol(2, -7, -8, 1, 9, dots_matrix);
        draw_symbol(2, 7, -8, 1, 9, dots_matrix);

            glColor3f(0.3f, 0.15f, 0.0f);
        draw_symbol(st.wHour / 10, -19, -8, 5, 9, digit_matrix);
        draw_symbol(st.wHour % 10, -13, -8, 5, 9, digit_matrix);
        draw_symbol(st.wMinute / 10, -5, -8, 5, 9, digit_matrix);
        draw_symbol(st.wMinute % 10, 1, -8, 5, 9, digit_matrix);

        if (st.wSecond % 2 == 1)
        {
            //glColor3f(0.27f, 0.17, 0.1f);
            glColor3f(0.2f, 0.25f, 0.15f);
            draw_symbol(st.wDayOfWeek, -13, 3, 7, 5, w_deys_matrix);
            draw_symbol(st.wDay / 10, -4, 2, 4, 7, m_deys_matrix);
            draw_symbol(st.wDay % 10, 2, 2, 4, 7, m_deys_matrix);
//                        g_start_flag = FALSE;
        }
        // Seconds
        draw_symbol(st.wSecond / 10, 9, -8, 5, 9, digit_matrix);
        draw_symbol(st.wSecond % 10, 15, -8, 5, 9, digit_matrix);
        //Dot blink
        int dc = st.wSecond % 2;
        draw_symbol(dc, -7, -8, 1, 9, dots_matrix);
        draw_symbol(dc, 7, -8, 1, 9, dots_matrix);

        wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
        p_second = st.wSecond;
        //Sleep(10);
    }

    int c_count = (st.wSecond == 0) ? 20 : g_start_flag;

    // background grey circles
    if(!c_count)
    {
        float grey = rand()%10 * 0.01f;
        glColor3f(grey, grey, grey);
        //for(int gw = 0; gw < 10; gw++) {
        float ix = rand_range(-cx, cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, (float)ix, (float)iy, step);
        TBall_Draw(ball);
        //wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
        Sleep(100);
        //}
    }
    else
    {
        // color circles
        float rc = rand()%10 * 0.01;
//                for(int gw = 0; gw < c_count; gw++)
//                {
            float gc = rand()%20 * 0.01;
            glColor3f(0.15+rc, gc, 0.0f);
            float ix = rand_range(-cx, cx) * space;
            float iy = rand_range(-cy, cy) * space;
            TBall_Init(&ball, (float)ix, (float)iy, step);
            TBall_Draw(ball);
            //wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
            //Sleep(1);
//                }
    }
    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
    g_start_flag = 0;

}



BOOL CALLBACK MyInfoEnumProc(
  HMONITOR hMonitor,	// handle to display monitor
  HDC hdcMonitor,   	// handle to monitor DC
  LPRECT lprcMonitor,   // monitor intersection rectangle
  LPARAM dwData     	// data
//  TScreen* mons     	// data
)
{
    cout << "======== MonitorEnumProc START ===========" << endl;

    HMONITOR hMon;
    static MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HWND desktop = GetDesktopWindow();
    RECT s_size;
    GetWindowRect(desktop, &s_size);
    GetMonitorInfo(hMonitor, &mi);
    int width = mi.rcMonitor.right - mi.rcMonitor.left;
    int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

    cout << "=== GetMonitorInfo(hMonitor, &mi); ====" << endl;
    cout << "Monitor NUM = " << mon_count << endl;
    cout << "Resolution = " << width << " x " << height << endl;
    cout << "left = " << mi.rcMonitor.left << " top = " << mi.rcMonitor.top << endl;
    cout << "right = " << mi.rcMonitor.right << " bottom = " << mi.rcMonitor.bottom << endl;

    mons_screen[mon_count].koef = (float)width / height;
    mons_screen[mon_count].step = 1.0f / 92.0f ;
    mons_screen[mon_count].space = mons_screen[mon_count].step * 2.5f;  // Space
    mons_screen[mon_count].cy = 1 / mons_screen[mon_count].space;
    mons_screen[mon_count].cx = mons_screen[mon_count].cy * mons_screen[mon_count].koef;

    cout << "koef = " << mons_screen[mon_count].koef << endl;
    cout << "step = " << mons_screen[mon_count].step << endl;
    cout << "space = " << mons_screen[mon_count].space << endl;
    cout << "cx = " << mons_screen[mon_count].cx << endl;
    cout << "cy = " << mons_screen[mon_count].cy << endl;

    cout << "======== MonitorEnumProc END ===========\n" << endl;
    mon_count++;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "WindowsScreenSaverClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

//    width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//    height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    int m_num = GetSystemMetrics(SM_CMONITORS);

    TScreen mons[m_num];
    mons_screen = mons;

    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);

    EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "WindowsScreenSaverClass",
                          "Screen Watch OpenGL",
                          //WS_OVERLAPPEDWINDOW,
                          WS_POPUP | WS_VISIBLE,
                          0,      //CW_USEDEFAULT,
                          0,      //CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW);

    /* enable OpenGL for the window */

    koef = (float)width / height;
    step = 1.0f / 92.0f ;
    space = step * 2.5f;  // Space
    cy = 1 / space;
    cx = cy * koef;

    EnableOpenGL(hwnd, &hDC, &hRC);
    glLoadIdentity();
    float xy_scale = 0.9f;
    //float xy_scale = 1.2f;
    glOrtho(-xy_scale,xy_scale, -xy_scale,xy_scale, -1, 1);

    glScalef( 1 / koef, 1, 1);
    Screensaver_Init(hDC);

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
            //draw_clock(&hDC);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

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

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        //////////////
    }

    case WM_CTLCOLORBTN:

    case IDC_GOGITBTN:
        ShellExecute(hDlg, "Open", "https://github.com/rty65tt/watch.scr", (LPCTSTR)NULL, (LPCTSTR)NULL, SW_SHOW);
        break;
    case IDOK:
        EndDialog(hDlg, LOWORD(wParam));
        break;

    case IDCANCEL:
        EndDialog(hDlg, LOWORD(wParam));
        break;
    }
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}
