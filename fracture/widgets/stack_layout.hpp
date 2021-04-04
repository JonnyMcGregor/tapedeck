// A layout that allows pushing and popping widgets as layers. FILO.
#pragma once
#include "../components/key_press.hpp"
#include "../components/point.hpp"
#include "../components/widget.hpp"
#include <vector>

struct StackLayout : Widget {
    std::vector<Widget *> sub_widgets = {};

    StackLayout() {
    }

    void push_layer(Widget &sub_widget) {
        this->sub_widgets.push_back(&sub_widget);
    }

    void pop_layer() {
        this->sub_widgets.pop_back();
    }

    void process(std::vector<KeyPress> &keyboard_input) {}

    void render(Screen &screen) {
        //Renders layers on top of each other
        this->sub_widgets.back()->render(screen);
    }
};