#pragma once
#include "components/screen.hpp"
#include "components/widget.hpp"
#include "utils/terminal_control.hpp"
#include "utils/terminal_cursor.hpp"
#include "utils/terminal_formatting.hpp"
#include "utils/terminal_info.hpp"
#include <iostream>
#include <optional>

struct Fracture {
    // Store as a pointer to prevent 'object slicing'
    std::optional<Widget *> root_widget;
    std::vector<KeyPress> keyboard_input;

    Fracture(Widget &root_widget) {
        this->root_widget = &root_widget;
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
            keyboard_input.push_back(input.value());
        }
        this->root_widget.value()->process(keyboard_input);
    }

    void render_to_viewport() {
        // TODO: Remove the shrinkage here
        Screen viewport = Screen(terminal::get_width() - 1, terminal::get_height() - 2);

        this->root_widget.value()->render(viewport);

        terminal::cursor::move_to_top_left();
        for (int y = 0; y < viewport.height; y++) {
            for (int x = 0; x < viewport.width; x++) {
                ScreenCell sc = viewport.get_screen_cell(Point(x, y));
                render_screencell(sc);
            }
            terminal::cursor::carriage_return();
        }
    }

private:
    void render_screencell(ScreenCell &sc) {
        if (sc.style.foreground_colour) {
            terminal::set_foreground_colour(sc.style.foreground_colour.value());
        }
        if (sc.style.background_colour) {
            terminal::set_background_colour(sc.style.background_colour.value());
        }

        std::cout << sc.to_string();

        if (sc.style.foreground_colour) {
            terminal::reset_foreground_colour();
        }
        if (sc.style.background_colour) {
            terminal::reset_background_colour();
        }
    }
};
