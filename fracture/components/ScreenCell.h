#pragma once
#include "TextStyle.h"

struct ScreenCell {
    char32_t character = ' ';
    TextStyle style = TextStyle{};
};
