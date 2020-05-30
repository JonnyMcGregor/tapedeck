#include "../primitives/Point.h"
#include "../primitives/ScreenCell.h"
#include <string>
#include <vector>
using namespace std;

struct Screen {
    vector<vector<ScreenCell>> content = {};
    int width;
    int height;

    Screen() {
        Screen{0, 0};
    }

    Screen(Point p) {
        Screen(p.x, p.y);
    }

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

    void draw(Point p, char c) {
        content[p.y][p.x].character = c;
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