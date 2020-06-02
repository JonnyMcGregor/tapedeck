#include "../primitives/Point.h"
#include "../primitives/ScreenCell.h"
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

    void draw(Point p, Screen s) {
        for (int i = 0; i < s.height; i++) {
            draw(p, s.getRowAsString(i));
            p.translate(Point(0, 1));
        }
    }

    void draw(Point p, string s) {
        for (long unsigned int i = 0; i < s.size(); i++) {
            draw(p, s.at(i));
            p.translate(Point(1, 0));
        }
    }

    // Returns true on success, false if drawing out of bounds
    bool draw(Point p, char c) {
        if (p.x >= this->width || p.y >= this->height || p.x < 0 || p.y < 0) {
            return false;
        }
        content[p.y][p.x].character = c;
        return true;
    }

    // Returns a vector containing a string for each row
    vector<string> render() {
        vector<string> outvec(height);
        for (int y = 0; y < height; y++) {
            outvec[y] = getRowAsString(y);
        }
        return outvec;
    }

private:
    string getRowAsString(int y) {
        string row_string;
        for (ScreenCell sc : content[y]) {
            row_string += sc.character;
        }
        return row_string;
    }
};