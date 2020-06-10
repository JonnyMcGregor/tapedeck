#pragma once

// Downgrading to lower bit depths should be handled by TermControl.
// Keep all colour top-notch depth in here.
struct Colour {
    float red;
    float green;
    float blue;

    Colour(float red, float green, float blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }
};