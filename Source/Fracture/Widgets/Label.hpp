#pragma once
#include "../Components/Widget.hpp"
#include <string>

struct Label : Widget {
    std::string text;

    Label() {
        this->text = "";
    }

    Label(std::string newText) {
        this->text = newText;
    }

    void process(std::vector<KeyPress> &keyboardInput) {}

    void render(Screen &screen) {
        screen.draw(Point(), this->text);
    }
};
