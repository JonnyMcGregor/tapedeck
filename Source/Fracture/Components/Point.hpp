#pragma once

struct Point {
    int x;
    int y;

    Point() {
        this->x = 0;
        this->y = 0;
    }

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    static Point zero() {
        return Point(0, 0);
    }

    Point operator+(Point other) {
        return Point(this->x + other.x, this->y + other.y);
    }
};