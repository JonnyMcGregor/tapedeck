#include "Point.h"
#include "ScreenCell.h"
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

struct Screen {
    vector<vector<ScreenCell>> content = {};
    int width;
    int height;

    Screen() {}

    Screen(Point p) : Screen(p.x, p.y) {}

    Screen(int width, int height) {
        content = vector<vector<ScreenCell>>(height); // Give size to outer vector
        for (int y = 0; y < height; y++) {
            content[y] = vector<ScreenCell>(width); // Initialise the width of each row
            for (int x = 0; x < width; x++) {
                content[y][x] = ScreenCell{}; // Initialise each character of the row
            };
        };
        this->width = width;
        this->height = height;
    }

    ScreenCell getCell(Point p) {
        if (!isValidCoordinates(p)) {
            throw invalid_argument("Coordinates lie outside of screen");
        }
        return content[p.y][p.x];
    }

    void draw(Point p, Screen screen) {
        for (int y = 0; y < screen.height; y++) {
            for (int x = 0; x < screen.width; x++) {
                draw(Point(p.x + x, p.y + y), screen.getCell(Point(x, y)));
            }
        }
    }

    bool draw(Point p, ScreenCell screencell) {
        if (!isValidCoordinates(p)) {
            return false;
        }
        content[p.y][p.x] = screencell;
        return true;
    }

    void draw(Point p, u32string s) {
        for (long unsigned int i = 0; i < s.size(); i++) {
            draw(p, s.at(i));
            p.translate(Point(1, 0));
        }
    }

    void draw(Point p, string s) {
        for (long unsigned int i = 0; i < s.size(); i++) {
            draw(p, s.at(i));
            p.translate(Point(1, 0));
        }
    }

    // Returns true on success, false if drawing out of bounds
    bool draw(Point p, char32_t c) {
        if (!isValidCoordinates(p)) {
            return false;
        }
        content[p.y][p.x].character = c;
        return true;
    }

    // Returns a vector containing a string for each row
    vector<u32string> render() {
        vector<u32string> outvec(height);
        for (int y = 0; y < height; y++) {
            outvec[y] = getRowAsString(y);
        }
        return outvec;
    }

    // private:
    bool isValidCoordinates(Point p) {
        if (p.x >= this->width || p.y >= this->height || p.x < 0 || p.y < 0) {
            return false;
        }
        return true;
    }

    u32string getRowAsString(int y) {
        u32string row_string;
        for (ScreenCell sc : content[y]) {
            row_string += sc.character;
        }
        return row_string;
    }
};