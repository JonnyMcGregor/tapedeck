#pragma once
#include <stdexcept>

struct Colour {
    float red;
    float green;
    float blue;

    Colour() {
        this->red = 0;
        this->green = 0;
        this->blue = 0;
    }

    Colour(float red, float green, float blue) {
        assertIsValidChannelValue(red);
        assertIsValidChannelValue(green);
        assertIsValidChannelValue(blue);
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

private:
    void assertIsValidChannelValue(float channelValue) {
        if (!isValidChannelValue(channelValue)) {
            throw std::invalid_argument("Channel value is out of bounds");
        }
    }

    bool isValidChannelValue(float channel) {
        if (0 <= channel && channel <= 1) {
            return true;
        }
        return false;
    };
};