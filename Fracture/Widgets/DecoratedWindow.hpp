#pragma once
#include "../Components/Point.hpp"
#include "../Components/Widget.hpp"
#include <iostream>
#include <optional>
#include <string>

struct DecoratedWindow : Widget {
    std::string title;
    std::optional<Widget *> subWidget;

    DecoratedWindow() {
        this->title = "";
    }

    DecoratedWindow(std::string newTitle) {
        this->title = newTitle;
    }

    // Takes a reference
    void set_sub_widget(Widget &subWidget) {
        // Set this->subWidget pointer to point to the _address_ of subWidget.
        this->subWidget = &subWidget;
    }

    void process(std::vector<KeyPress> &keyboardInput) {}

    void render_sub_widget(Screen &screen) {
        if (this->subWidget) {
            this->subWidget.value()->render(screen);
        };
    }

    void render(Screen &screen) {
        Screen sub_screen = Screen(screen.width - 2, screen.height - 2);
        this->render_sub_widget(sub_screen);

        screen.draw(Point(1, 1), sub_screen);

        u_short right = screen.width - 1;
        u_short bottom = screen.height - 1;

        for (int x = 0; x < screen.width; x++) {
            screen.draw(Point(x, 0), U'─');
            screen.draw(Point(x, bottom), U'─');
        };
        for (int y = 0; y < screen.height; y++) {
            screen.draw(Point(0, y), U'│');
            screen.draw(Point(right, y), U'│');
        };
        screen.draw(Point(0, 0), U'┌');
        screen.draw(Point(right, 0), U'┐');
        screen.draw(Point(0, bottom), U'└');
        screen.draw(Point(right, bottom), U'┘');

        // Don't draw title if title is empty
        if (this->title == "") {
            return;
        };

        // Add a gap to either side of the title
        std::string title = ' ' + this->title + ' ';
        // Calculate the horizontal offset needed to shift the title to
        //  the center of the frame
        int offset = ((screen.width - 2) - title.length()) / 2;
        screen.draw(Point(offset, 0), title);
    };
};