#include "components/Colour.h"
#include "components/Style.h"
#include "components/Window.h"
#include <codecvt>
#include <iostream>
#include <locale>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <termios.h>
#include <unistd.h> // for STDOUT_FILENO
#include <vector>

using namespace std;

enum struct Direction {
    Up,
    Down,
    Left,
    Right,

};

struct TermSize {
    static Point getSize() {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return Point(size.ws_col, size.ws_row);
    };
    static int getWidth() {
        return getSize().x;
    };
    static int getHeight() {
        return getSize().y;
    }
};

struct TermControl {
    static void setForegroundColour(Colour colour) {
        cout << "\e[3" + to_string(getColourValue(colour)) + "m";
    }
    static void setBackgroundColour(Colour colour) {
        cout << "\e[4" + to_string(getColourValue(colour)) + "m";
    }
    static void resetColours() {
        setForegroundColour(Colour::Reset);
        setBackgroundColour(Colour::Reset);
    }
    static void resetAll() {
        cout << "\e[0m";
    }

    static void moveCursorToLineStart() {
        cout << "\e[1E";
    }

    static void moveCursorToTopLeft() {
        moveCursorUp(TermSize::getHeight() - 1);
    }

    static void scrollBack(int num_lines) {
        cout << "\e[" + to_string(num_lines) + "T";
    }

    static void moveCursorToPoint(Point p) {
        p.translate(Point(1, 1));
        cout << "\e[" + to_string(p.x) + ";" + to_string(p.y) + "G";
    }

    static void moveCursorUp(int num_lines) {
        cout << "\e[" + to_string(num_lines) + "F";
    }

    static void moveCursorToLineEnd() {
        cout << "\e[1E";
    }

    static void clearScreen() {
        cout << "\e[2J";
    }

    static void clearLine() {
        cout << "\e[2K";
    }

    static void setCursorVisible(bool visible) {
        if (visible) {
            cout << "\e[?25h";
        } else {
            cout << "\e[?25l";
        }
    }

    static void
    moveCursor(Direction direction, unsigned int distance) {
        char direction_char;
        switch (direction) {
        case Direction::Up:
            direction_char = 'A';
        case Direction::Down:
            direction_char = 'B';
        case Direction::Left:
            direction_char = 'C';
        case Direction::Right:
            direction_char = 'D';
        }
        cout << "\e[" + to_string(distance) + direction_char;
    }

private:
    static int getColourValue(Colour colour) {
        return static_cast<int>(colour);
    }
};

int getch(void) {
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
};

/* One Fracture class should exist in a program. It holds the viewport Screen,
   as well as a vector of Windows. The viewport and all windows will resize
   every time the terminal is resized. */
struct Fracture {
    Screen viewport = {0, 0};
    vector<Window *> windows = {};
    Fracture() {
        viewport = Screen(TermSize::getSize());
    }

    void addWindow(Window &window) {
        windows.push_back(&window);
    }

    void flattenWindowsToViewport() {
        for (Window *w : windows) {
            viewport.draw(w->rect.getOrigin(), w->render());
        }
    }

    void render() {
        flattenWindowsToViewport();
        wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
        vector<u32string> rendered_viewport = viewport.render();

        for (int y = 0; y < viewport.height; y++) {
            for (int x = 0; x < viewport.width; x++) {
                cout << converter.to_bytes(viewport.content[y][x].character);
            }
        }
    }
};
