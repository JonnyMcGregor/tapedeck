#pragma once
#include "../components/widget.hpp"
#include <string>

struct Label : Widget {
    std::string text;

    Label() {
        this->text = "";
    }

    Label(std::string new_text) {
        this->text = new_text;
    }

    void process(std::vector<KeyPress> &keyboard_input) {}

    void render(Screen &screen) {
        screen.draw(Point(), this->text);
    }
};
