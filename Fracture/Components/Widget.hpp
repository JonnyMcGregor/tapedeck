#pragma once
#include "KeyPress.hpp"
#include "Screen.hpp"

struct Widget {
    // Declared virtual to prevent 'object slicing'
    // https://stackoverflow.com/questions/274626/what-is-object-slicing
    virtual void render(Screen &screen) = 0;
    virtual void process(std::vector<KeyPress> &keyboardInput) = 0;
    bool is_selected = false;
    ScreenCellStyle selectedWidgetStyle;
};