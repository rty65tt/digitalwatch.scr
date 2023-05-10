
//#define PI       3.14159265358979323846   // pi
#define TIMER           1
#define XYSCALE         0.9
#define CIRCLE_EDGES    16

HINSTANCE    hMainInstance;

static int g_start_flag = 400;

static SYSTEMTIME st;
static int p_second = 0;

static int mouse_x_init = 0;
static int mon_count = 0;
static int m_num = 0;

#define CIRCLES_NUM     71
static float space = 2.0 / CIRCLES_NUM;
static float step = space * 0.4f;
static int cy = 1.0 / space;

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
