// A layout that allows pushing and popping widgets as layers. FILO.
#pragma once
#include "../Components/KeyPress.hpp"
#include "../Components/Point.hpp"
#include "../Components/Widget.hpp"
#include <vector>

struct StackLayout : Widget {
    std::vector<Widget *> subWidgets = {};

    StackLayout() {
    }

    void push_layer(Widget &subWidget) {
        this->subWidgets.push_back(&subWidget);
    }

    void pop_layer() {
        this->subWidgets.pop_back();
    }

    void process(std::vector<KeyPress> &keyboardInput) {}

    void render(Screen &screen) {
        //Renders layers on top of each other
        this->subWidgets.back()->render(screen);
    }
};