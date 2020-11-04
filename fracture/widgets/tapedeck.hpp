#pragma once
#include "../components/point.hpp"
#include "../components/widget.hpp"
#include "decorated_window.hpp"
#include <iostream>
#include <optional>
#include <string>

struct TapeDeck : Widget {
    DecoratedWindow sub_widget;

    TapeDeck() {
        DecoratedWindow dw = DecoratedWindow("title");
        this->sub_widget = dw;
    }

    void process(std::vector<KeyPress> &keyboard_input) {
        if (keyboard_input.size() > 0) {
            KeyPress b = keyboard_input.back();
            if (keyboard_input.back() == KeyPress(Key::K_Q, ModifierKey::Control)) {
                exit(1);
            }
        }
    }

    void render(Screen &screen) {
        this->sub_widget.render(screen);
    };
};
