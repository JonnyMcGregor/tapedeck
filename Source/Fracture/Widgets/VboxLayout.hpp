#pragma once

#include "../Components/Point.hpp"
//#include "../Components/Widget.hpp"
#include <vector>
#include <memory>
//
//struct VBoxLayout : Widget {
//    std::vector<Widget*> subWidgets;
//
//    VBoxLayout() {
//    }
//
//    void add_sub_widget(Widget &subWidget) {
//        this->subWidgets.push_back(&subWidget);
//    }
//
//    void process(std::vector<KeyPress> &keyboardInput) {}
//
//    void render(Screen &screen) {
//        if (this->subWidgets.size() == 0) {
//            return;
//        }
//        u_short widget_y = 0;
//        u_short widget_height = screen.height / this->subWidgets.size();
//
//        for (u_short i = 0; i < this->subWidgets.size(); i++) {
//            Screen sub_widget_screen = Screen(screen.width, widget_height);
//            this->subWidgets[i]->render(sub_widget_screen);
//            screen.draw(Point(0, widget_y), sub_widget_screen);
//            widget_y += widget_height;
//        };
//    }
//};
