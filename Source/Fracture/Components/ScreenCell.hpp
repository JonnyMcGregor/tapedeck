#pragma once
#include "ScreenCellStyle.hpp"
#include <codecvt>
#include <locale>
#include <optional>

struct ScreenCell {
    char32_t character;
    ScreenCellStyle style;

    ScreenCell() {
        this->character = ' ';
        this->style = ScreenCellStyle();
    }

    ScreenCell(char32_t character) {
        this->character = character;
    }

    std::string to_string() const {
        using namespace std;
        wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
        return converter.to_bytes(this->character);
    }
};