#pragma once
#include "TextStyle.h"

struct ScreenCell {
    char32_t character = ' ';
    TextStyle style = TextStyle{};

    ScreenCell() {}

    ScreenCell(char32_t character, TextStyle style) {
        this->character = character;
        this->style = style;
    }
};
