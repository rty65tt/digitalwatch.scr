#include <Windows.h>
// OpenGL Includes
#include <gl\GL.h>
#include <gl\GLU.h>
// Screensaver Includes
#include <ScrnSave.h>
#include <CommCtrl.h>
#include <math.h>

//#include <iostream>
//
//using namespace std;

//// OpenGL Libraries
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")
//// Screensaver Libraries
//#pragma comment(lib, "scrnsave.lib")
//#pragma comment(lib, "ComCtl32.lib")

#define PI       3.14159265358979323846   // pi
#define TIMER   1

extern HINSTANCE    hMainInstance;      // screen saver instance handle
extern BOOL         fChildPreview;

int width;
int height;
float koef;

static SYSTEMTIME st;
static bool g_start_flag = TRUE;

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


void Game_Init(HDC hDC)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);


    for (float iy=-cy; iy < cy; iy++)
    {
        for (float ix=-cx; ix < cx; ix++)
        {
            float grey = rand()%10  * 0.01f ;
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

    glColor3f(0.05f, 0.05f, 0.05f);
    if (st.wSecond % 2 == 0 || g_start_flag)
    {
        draw_symbol(8, -19, -9, 5, 9, digit_matrix);
        draw_symbol(8, -13, -9, 5, 9, digit_matrix);
        draw_symbol(8, -5, -9, 5, 9, digit_matrix);
        draw_symbol(8, 1, -9, 5, 9, digit_matrix);
    }
    draw_symbol(8, 9, -9, 5, 9, digit_matrix);
    draw_symbol(8, 15, -9, 5, 9, digit_matrix);

    draw_symbol(2, -7, -9, 1, 9, dots_matrix);
    draw_symbol(2, 7, -9, 1, 9, dots_matrix);

    glColor3f(0.17f, 0.15f, 0.15f);
    if (st.wSecond % 2 == 0 || g_start_flag)
    {
        g_start_flag = FALSE;

        draw_symbol(st.wHour / 10, -19, -9, 5, 9, digit_matrix);
        draw_symbol(st.wHour % 10, -13, -9, 5, 9, digit_matrix);
        draw_symbol(st.wMinute / 10, -5, -9, 5, 9, digit_matrix);
        draw_symbol(st.wMinute % 10, 1, -9, 5, 9, digit_matrix);

        glColor3f(0.15f, 0.14f, 0.14f);
        draw_symbol(st.wDayOfWeek, -13, 2, 7, 5, w_deys_matrix);
        draw_symbol(st.wDay / 10, -4, 2, 4, 7, m_deys_matrix);
        draw_symbol(st.wDay % 10, 2, 2, 4, 7, m_deys_matrix);

    }

    draw_symbol(st.wSecond / 10, 9, -9, 5, 9, digit_matrix);
    draw_symbol(st.wSecond % 10, 15, -9, 5, 9, digit_matrix);

    int dc = st.wSecond % 2;
    draw_symbol(dc, -7, -9, 1, 9, dots_matrix);
    draw_symbol(dc, 7, -9, 1, 9, dots_matrix);

    wglSwapLayerBuffers(*hDC, WGL_SWAP_MAIN_PLANE);

    Sleep(10);

    float grey = rand()%10 * 0.01f;
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

    static UINT uTimer = 0;

    switch (uMsg)
    {
    case WM_CREATE:
    {

        EnableOpenGL(hwnd, &hDC, &hRC);
        width = rc.right;
        height = rc.bottom;

        koef = width > height ? (float)width / height: (float)height / width;
        //koef = (float)width / height;
        //        cout << "RECT " << rc.right << " x " << rc.bottom << endl;
        //        cout << "RECT " << mi.rcMonitor.right << " x " << mi.rcMonitor.bottom << endl;
        //        cout << "RECT " << width << " x " << height << endl;

        step = 1.0f / 64.0f ;
        space = step * 2.5f;  // Space
        cy = 1 / space;
        cx = cy * koef;

        glLoadIdentity();
        float xy_scale = 0.9f;
        glOrtho(-xy_scale,xy_scale, -xy_scale,xy_scale, -1, 1);

        //float sc =

        glScalef( 1 / koef, 1, 1);

        Game_Init(hDC);

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
        //PostQuitMessage(0);
    }
    break;

    case WM_KEYDOWN:
    {
        PostQuitMessage(0);
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
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

void Update()
{
    //Sleep(1);
}
