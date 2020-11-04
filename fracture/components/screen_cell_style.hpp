#pragma once
#include "colour.hpp"
#include <optional>

struct ScreenCellStyle {
    std::optional<Colour> foreground_colour;
    std::optional<Colour> background_colour;

    ScreenCellStyle operator+(const ScreenCellStyle &other) const {
        ScreenCellStyle new_style = ScreenCellStyle();
        if (this->foreground_colour) {
            new_style.foreground_colour = this->foreground_colour;
        } else {
            new_style.foreground_colour = other.foreground_colour;
        };

        if (this->background_colour) {
            new_style.background_colour = this->background_colour;
        } else {
            new_style.background_colour = other.background_colour;
        };

        return new_style;
    }
};