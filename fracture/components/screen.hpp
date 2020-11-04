#pragma once
#include "point.hpp"
#include "screen_cell.hpp"
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

struct Screen {
    u_short width;
    u_short height;
    vector<vector<ScreenCell>> content;

    Screen() {}

    Screen(u_short width, u_short height) {

        this->width = width;
        this->height = height;

        // Set size of outer vector
        this->content = vector<vector<ScreenCell>>(height);
        for (int y = 0; y < height; y++) {
            // Initialise the width of each row
            this->content[y] = vector<ScreenCell>(width);
            for (int x = 0; x < width; x++) {
                // Initialise each character of the row
                this->set_screen_cell(Point(x, y), ScreenCell{U' '});
            };
        };
    }

    ScreenCell get_screen_cell(Point p) {
        assert_is_valid_screen_coordinates(p);
        return this->content[p.y][p.x];
    }

    void set_screen_cell(Point p, ScreenCell sc) {
        assert_is_valid_screen_coordinates(p);
        this->content[p.y][p.x] = sc;
    }

    bool draw(Point p, ScreenCell sc) {
        if (!is_valid_screen_coordinates(p)) {
            return false;
        };
        this->set_screen_cell(p, sc);
        return true;
    }

    bool draw(Point p, char32_t new_character) {
        if (!is_valid_screen_coordinates(p)) {
            return false;
        };
        this->content[p.y][p.x].character = new_character;
        return true;
    }

    bool draw(Point p, string text) {
        bool success_state = true;
        for (int i = 0; i < text.length(); i++) {
            if (!this->draw(Point(i, 0) + p, text[i])) {
                success_state = false;
            }
        }
        return success_state;
    }

    bool draw(Point p, Screen &screen) {
        // Draws a Screen onto this screen, with the top-left corner at the
        // specified point. Returns `true` if provided screen is fully contained
        // within this screen, `false` otherwise

        bool success_state = true;
        ScreenCell transplant;

        for (u_short y = 0; y < screen.height; y++) {
            for (u_short x = 0; x < screen.width; x++) {
                if (!is_valid_screen_coordinates(p + Point(x, y))) {
                    success_state = false;
                    continue;
                }
                transplant = screen.get_screen_cell(Point(x, y));
                this->set_screen_cell(Point(x, y) + p, transplant);
            }
        }
        return success_state;
    }

    void set_style(ScreenCellStyle style) {
        for (u_short y = 0; y < this->height; y++) {
            for (u_short x = 0; x < this->width; x++) {
                ScreenCell sc = this->get_screen_cell(Point(x, y));
                sc.style = sc.style + style;
                this->set_screen_cell(Point(x, y), sc);
            }
        }
    }

private:
    bool is_valid_screen_coordinates(Point p) {
        if (p.x >= this->width || p.y >= this->height || p.x < 0 || p.y < 0) {
            return false;
        };
        return true;
    }
    void assert_is_valid_screen_coordinates(Point p) {
        if (!is_valid_screen_coordinates(p)) {
            throw invalid_argument("Coordinates are out of bounds");
        }
    }
};