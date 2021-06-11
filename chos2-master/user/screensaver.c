#include <fast_hsv2rgb.h>

int hue = HSV_HUE_MIN;
int val = HSV_VAL_MAX;
int dir = -3;
char r, g, b;

for (int i=0;;i++) {

    if (i < 10000) continue;;
    i = 0;

    hue++;
    if(hue > HSV_HUE_MAX) hue -= HSV_HUE_MAX;

    fast_hsv2rgb_32bit(hue, HSV_SAT_MAX * 2 / 3, val, &r, &g, &b);

    bwprintf(COM2, "\033[48;2;%d;%d;%dm\r\n", r, g, b);
}