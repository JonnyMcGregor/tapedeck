#pragma once
//#include "Colour.hpp"
//#include <optional>
//
//struct ScreenCellStyle {
//    std::optional<Colour> foregroundColour;
//    std::optional<Colour> backgroundColour;
//
//    ScreenCellStyle operator+(const ScreenCellStyle &other) const {
//        ScreenCellStyle new_style = ScreenCellStyle();
//        if (this->foregroundColour) {
//            new_style.foregroundColour = this->foregroundColour;
//        } else {
//            new_style.foregroundColour = other.foregroundColour;
//        };
//
//        if (this->backgroundColour) {
//            new_style.backgroundColour = this->backgroundColour;
//        } else {
//            new_style.backgroundColour = other.backgroundColour;
//        };
//
//        return new_style;
//    }
//};