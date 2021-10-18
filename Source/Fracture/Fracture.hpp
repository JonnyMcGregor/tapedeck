#pragma once
#include "Components/Screen.hpp"
#include "Components/Widget.hpp"
#include "Terminal/terminal_control.hpp"
#include "Terminal/terminal_cursor.hpp"
#include "Terminal/terminal_formatting.hpp"
#include "Terminal/terminal_info.hpp"
#include <iostream>
#include <optional>

struct Fracture {
    // Store as a pointer
    std::optional<Widget *> rootWidget;
    std::vector<KeyPress> keyboardInput;

    Fracture(Widget &rootWidget) {
        this->rootWidget = &rootWidget;
        terminal::enable_raw_mode();
        terminal::cursor::hide();

        // Scroll display to clear it
        for (int y = 0; y < terminal::get_height(); y++) {
            terminal::cursor::line_feed();
        }
    }

    ~Fracture() {
        terminal::disable_raw_mode();
        terminal::cursor::show();
    }

    void process() {
        std::optional<KeyPress> input = terminal::get_key();
        if (input) {
            keyboardInput.push_back(input.value());
        }
        this->rootWidget.value()->process(keyboardInput);
    }

    void renderToViewport() {
        // TODO: Remove the shrinkage here
        Screen viewport = Screen(terminal::get_width() - 1, terminal::get_height() - 2);

        this->rootWidget.value()->render(viewport);

        terminal::cursor::move_to_top_left();
        for (int y = 0; y < viewport.height; y++) {
            for (int x = 0; x < viewport.width; x++) {
                ScreenCell sc = viewport.get_screen_cell(Point(x, y));
                renderScreencell(sc);
            }
            terminal::cursor::carriage_return();
        }
    }

private:
    void renderScreencell(ScreenCell &sc) {
        if (sc.style.foregroundColour) {
            terminal::set_foreground_colour(sc.style.foregroundColour.value());
        }
        if (sc.style.backgroundColour) {
            terminal::set_background_colour(sc.style.backgroundColour.value());
        }

        std::cout << sc.to_string();

        if (sc.style.foregroundColour) {
            terminal::reset_foreground_colour();
        }
        if (sc.style.backgroundColour) {
            terminal::reset_background_colour();
        }
    }
};
