#include <Windows.h>
#include <Windowsx.h>
#include <winuser.h>
// OpenGL Includes
#include <gl\GL.h>
#include <gl\GLU.h>
// Screensaver Includes
#include <ScrnSave.h>
#include <CommCtrl.h>
#include <math.h>
#include "main.h"

#include "resource.h"

#include <iostream>
using namespace std;

//// OpenGL Libraries
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")
//// Screensaver Libraries
//#pragma comment(lib, "scrnsave.lib")
//#pragma comment(lib, "ComCtl32.lib")

extern HINSTANCE    hMainInstance;      // screen saver instance handle
extern BOOL         fChildPreview;

int width;
int height;
float koef;
static int cx;

//LONG WINAPI ScreenSaverProc(HWND, UINT, WPARAM, LPARAM);
//BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void Render(HDC*);
void Update();
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
    int left, right, top, bottom;
    int width, height;
    float koef;
    int cx;
} TScreen;

TScreen mons_screen[100];

typedef struct
{
    float x,y;
    float r;
} TBall;

TBall ball;

void TBall_Init(TBall *obj, float x1, float y1, float r1)
{
    obj->x = x1+r1;
    obj->y = y1+r1;
    obj->r = r1;
}

void TBall_Show(TBall obj)
{
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0);
    glScalef(obj.r, obj.r, 1);
    //drawSquareFill();
    drawCircleFill(CIRCLE_EDGES);
    glPopMatrix();
}

void draw_symbol(unsigned int m, int x, int y, int cel, int row, char *p_matrix)
{
    int bm_x = 0;
    int bm_y = 0;
    int counter;
    char *p_2char = &p_matrix[cel * row * m];
    char *p_bm = p_2char;

    if(!keydown && rand()%50 < 1) {
        glColor3f(grey, grey, grey);
    }

    while (1)
    {
        counter = 0;
        do
        {
            if (p_bm[counter] != 48)
            {
                float ix = (x + bm_x - x_offst) * space;
                float iy = (y + bm_y - y_offst) * -space;
                TBall_Init(&ball, ix, iy, step);
                TBall_Show(ball);
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
    for (int iy=-cy; iy < cy; iy++)
    {
        for (int ix=-cx; ix < cx; ix++)
        {
            grey = rand()%10  * 0.01f ;
            glColor3f(grey, grey, grey);
            TBall_Init(&ball, (float)ix*space, (float)iy*space, step);
            TBall_Show(ball);
        }
    }
    wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
}

float rand_range(int range_min, int range_max)
{
    int r = ((double)rand() / RAND_MAX) * (range_max - range_min) + range_min;
    return r;
}

void draw_clock(HDC *hDC)
{
    glColor3f(0.05f, 0.05f, 0.05f);

    keydown=TRUE;
    if (st.wSecond % 2 == 0 || g_start_flag)
    {
        draw_symbol(8, -19, -9, 5, 9, digit_matrix);
        draw_symbol(8, -13, -9, 5, 9, digit_matrix);
        draw_symbol(8, -5, -9, 5, 9,  digit_matrix);
        draw_symbol(8, 1, -9, 5, 9,   digit_matrix);
    }
    draw_symbol(8, 9, -9, 5, 9,       digit_matrix);
    draw_symbol(8, 15, -9, 5, 9,      digit_matrix);

    draw_symbol(2, -7, -9, 1, 9,      dots_matrix);
    draw_symbol(2, 7, -9, 1, 9,       dots_matrix);
    keydown=FALSE;

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

    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);
}

void Render(HDC *hDC)
{
    /* OpenGL animation code goes here */

    float rc = rand()%30 * 0.01;
    float gc = rand()%20 * 0.01;
    glColor3f(rc, gc, 0.0f);
    float ix = rand_range(-cx, cx) * space;
    float iy = rand_range(-cy, cy) * space;
    TBall_Init(&ball, (float)ix, (float)iy, step);
    TBall_Show(ball);
    Sleep(10);

    GetLocalTime(&st);

    draw_clock(hDC);

    Sleep(10);

    grey = rand()%10 * 0.01f;
    glColor3f(grey, grey, grey);
    for(int gw = 0; gw < 10; gw++)
    {
        float ix = rand_range(-cx, cx) * space;
        float iy = rand_range(-cy, cy) * space;
        TBall_Init(&ball, (float)ix, (float)iy, step);
        TBall_Show(ball);
        Sleep(10);
    }
}

BOOL CALLBACK MyInfoEnumProc(
  HMONITOR hMonitor,	// handle to display monitor
  HDC hdcMonitor,   	// handle to monitor DC
  LPRECT lprcMonitor,   // monitor intersection rectangle
  LPARAM dwData     	// data
)
{
        HWND hwnd;
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
        cout << "Resolution = " << width << " x " << height << endl;
        cout << "rect " << mi.rcMonitor.left << ", " << mi.rcMonitor.top << ", " << mi.rcMonitor.right << ", " << mi.rcMonitor.bottom << endl;

        mons_screen[mon_count].koef = width > height ? (float)width / height : (float)height / width;
        mons_screen[mon_count].cx = cy * mons_screen[mon_count].koef;
        mons_screen[mon_count].hwnd = hwnd;
        mons_screen[mon_count].width = width;
        mons_screen[mon_count].height = height;
        mons_screen[mon_count].left = mi.rcMonitor.left;
        mons_screen[mon_count].top = mi.rcMonitor.top;
        mons_screen[mon_count].right = mi.rcMonitor.right;
        mons_screen[mon_count].bottom = mi.rcMonitor.bottom;
        cout << "= " << mons_screen[mon_count].width << ", " << mons_screen[mon_count].height << ", " << mons_screen[mon_count].left << ", " << mons_screen[mon_count].right << ", " << mons_screen[mon_count].top << ", " << mons_screen[mon_count].bottom << endl;

//    cout << "=== MonitorEnumProc END = Mon # " << mon_count << " ============\n" << endl;

    //mon_count = mon_count <= m_num ? mon_count+1 : 0;
    mon_count++;
    return TRUE;
}


LRESULT WINAPI ScreenSaverProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC;
    HGLRC hRC;
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    static MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hmon, &mi);
    hDC = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);

    m_num = GetSystemMetrics(SM_CMONITORS);

    //TScreen mons[m_num];
    //mons_screen = mons;


    static UINT uTimer = 0;

    switch (uMsg)
    {
        case WM_CREATE:
        {
            EnableOpenGL(hwnd, &hDC, &hRC);
            width = rc.right;
            height = rc.bottom;
//            EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);
            //SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, mons_screen[0].width, mons_screen[0].height, SWP_SHOWWINDOW);

            koef = width > height ? (float)width / height: (float)height / width;

//            cout << "RECT " << rc.right << " x " << rc.bottom << endl;
//            cout << "RECT " << mi.rcMonitor.right << " x " << mi.rcMonitor.bottom << endl;
//            cout << "RECT " << width << " x " << height << endl;

            cx = cy * koef;
            glLoadIdentity();
            glOrtho(-XYSCALE, XYSCALE, -XYSCALE, XYSCALE, -1, 1);
            //glOrtho(-width/2, width/2, -height/2, height/2, -1, 1);
            //glScalef( width / 2 / koef, height/2, 1);
            glScalef(1/koef, 1, 1);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            Screensaver_Init(hDC);
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
            //Screensaver_Init(hDC);
            switch (wParam) {
                case VK_UP:     {y_offst++;keydown=TRUE;}break;
                case VK_DOWN:   {y_offst--;keydown=TRUE;}break;
                case VK_LEFT:   {x_offst++;keydown=TRUE;}break;
                case VK_RIGHT:  {x_offst--;keydown=TRUE;}break;
                case VK_BACK:  {Screensaver_Init(hDC);}break;
                default: {PostQuitMessage(0);}
            }
        }
        break;

        case WM_KEYUP:
        {
            keydown=FALSE;
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
    static HWND hCheckbox, previewer;
    static COLORREF selectcolor;
    static RECT         rc;

    GetClientRect(hDlg, &rc);

    switch (message) {
        case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_GOGITBTN:
            ShellExecute(hDlg, "Open", "https://github.com/rty65tt/stClock.scr", (LPCTSTR)NULL, (LPCTSTR)NULL, SW_SHOW);
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
}


BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

void Update()
{
    //Sleep(1);
}
