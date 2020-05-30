#include "Screen.h"
struct Rect {
    Rect(int origin_x, int origin_y, int width, int height) {
        x = origin_x;
        y = origin_y;
        this->width = width;
        this->height = height;
    }

    int x = 0, y = 0, width = 0, height = 0;
};

struct ScreenSpaceRect : Rect {
    ScreenSpaceRect(int ss_left, int ss_top, int ss_width, int ss_height, Screen parent) {
        if (ss_left >= 0)
            x = ss_left;
        else
            x = parent.width - ss_left;
        if (ss_top >= 0)
            y = ss_top;
        else
            y = parent.height - ss_top;
        if (ss_width >= 0)
            width = ss_width;
        else {
            int right = ss_width + parent.width + 1;
            width = right - ss_left;
        }
        if (ss_height >= 0)
            height = ss_height;
        else {
            int bottom = ss_height + parent.height + 1;
            height = bottom - ss_top;
        }
    }
};