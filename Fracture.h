#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <vector>

using namespace std;

struct Fracture {
    winsize getSize() {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size;
    };
    int getWidth() {
        return getSize().ws_col;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns.
     */
    };
    int getHeight() {
        return getSize().ws_row;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns.
     */
    }
};

enum struct Colour {
    black = 0,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    reset = 9,
};

enum struct Style {
    bold = 1,
    underlined = 2,
    faint = 4,
    reversed = 8,
};

struct TextFormat {
    static void setForegroundColour(Colour colour) {
        std::cout << "\033[3" + std::to_string(getColourValue(colour)) + "m";
    }
    static void setBackgroundColour(Colour colour) {
        std::cout << "\033[4" + std::to_string(getColourValue(colour)) + "m";
    }
    static void resetColours() {
        setForegroundColour(Colour::reset);
        setBackgroundColour(Colour::reset);
    }
    static void resetAll() {
        std::cout << "\033[0m";
    }

private:
    static int getColourValue(Colour colour) {
        return static_cast<int>(colour);
    }
};

struct ScreenCell {
    char character = ' ';
};

struct Point {
    int x;
    int y;

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void translate(Point p) {
        x = x + p.x;
        y = y + p.y;
    }
};

struct Screen {
    vector<vector<ScreenCell>> content = {};
    int width;
    int height;

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