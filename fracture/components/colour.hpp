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
        assert_is_valid_channel_value(red);
        assert_is_valid_channel_value(green);
        assert_is_valid_channel_value(blue);
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

private:
    void assert_is_valid_channel_value(float channel_value) {
        if (!is_valid_channel_value(channel_value)) {
            throw std::invalid_argument("Channel value is out of bounds");
        }
    }

    bool is_valid_channel_value(float channel) {
        if (0 <= channel && channel <= 1) {
            return true;
        }
        return false;
    };
};