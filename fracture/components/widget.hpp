#pragma once
#include "key_press.hpp"
#include "screen.hpp"

struct Widget {
    // Declared virtual to prevent 'object slicing'
    // https://stackoverflow.com/questions/274626/what-is-object-slicing
    virtual void render(Screen &screen) = 0;
    virtual void process(std::vector<KeyPress> &keyboard_input) = 0;
};