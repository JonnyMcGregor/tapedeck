#include "Screen.h"

struct Rect {
    Rect(){};

    Rect(int origin_x, int origin_y, int width, int height) {
        this->x = origin_x;
        this->y = origin_y;
        this->width = width;
        this->height = height;
    }

    Point getOrigin() {
        return Point(x, y);
    }

    int x = 0, y = 0, width = 0, height = 0;
};

struct ScreenSpaceRect : Rect {
    ScreenSpaceRect(int ss_left, int ss_top, int ss_width, int ss_height, Screen parent) {
        x = ss_left;
        y = ss_top;
        width = ss_width;
        height = ss_height;

        if (ss_left < 0) {
            this->x = parent.width + ss_left;
        }
        if (ss_top < 0) {
            this->y = parent.height + ss_top;
        }
        if (ss_width < 0) {
            int right = parent.width + ss_width + 1;
            width = right - x;
        }
        if (ss_height < 0) {
            int bottom = ss_height + parent.height + 1;
            height = bottom - ss_top;
        }
    }
};

struct CentredRect : Rect {
    CentredRect(int width, int height, Screen parent, Point offset = Point(0, 0)) {
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->parent = parent;
        recalculateBounds();
    }

    Point getParentCentre() {
        int centre_x = parent.width * 0.5;
        int centre_y = parent.height * 0.5;
        return Point(centre_x, centre_y);
    }

    void recalculateBounds() {
        int half_width = width * 0.5;
        int half_height = height * 0.5;
        x = getParentCentre().x - half_width + offset.x;
        y = getParentCentre().y - half_height + offset.y;
    }
    Point offset;
    Screen parent;
};