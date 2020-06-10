#pragma once
#include <cassert>
#include <cmath>
#include <cstdio>

using namespace std;
// Downgrading to lower bit depths should be handled by TermControl.
// Keep all colour top-notch depth in here.
struct Colour {
    float red;
    float green;
    float blue;

    Colour() {}

    bool operator==(Colour &other) {
        if (this->red != other.red) return false;
        if (this->green != other.green) return false;
        if (this->blue != other.blue) return false;
        return true;
    }

    Colour(float red, float green, float blue) {
        assert(0 <= red && red <= 1);
        assert(0 <= green && green <= 1);
        assert(0 <= blue && blue <= 1);
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    float getHypotenuse(Colour other) {
        float red_diff = abs(this->red - other.red);
        float green_diff = abs(this->green - other.green);
        float blue_diff = abs(this->blue - other.blue);
        return sqrt(pow(red_diff, 2) + pow(green_diff, 2) + pow(blue_diff, 2));
    }

    void print() {
        printf("%d %d %d\n", (int)(red * 255), (int)(green * 255), (int)(blue * 255));
    }
};