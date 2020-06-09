#pragma once
#include "Colour.h"
#include "Point.h"
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <sys/select.h>
#include <termios.h>
#include <unistd.h> // for STDOUT_FILENO

using namespace std;

enum struct Direction {
    Up,
    Down,
    Left,
    Right,

};

struct TermControl {

    static int getch() {
        setEcho(false);
        setCanonical(false);
        return getchar();
    };

    static int kbhit() {
        static const int STDIN = 0;
        static bool initialized = false;

        if (!initialized) {
            // Use termios to turn off line buffering
            setCanonical(false);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }

    static void setCanonical(bool canonical) {
        setStdinFlag(ICANON, canonical);
    }

    static void setEcho(bool echo) {
        setStdinFlag(ECHO, echo);
    }

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

    static void setTForegroundColour(int red, int green, int blue) {
        // int colour_code = getTrueColourCode(red, green, blue);
        cout << "\e[38;2;" << to_string(red) << ";" << to_string(green) << ";" << to_string(blue) << "m";
    }
    static void setTBackgroundColour(int red, int green, int blue) {
        // int colour_code = getTrueColourCode(red, green, blue);
        cout << "\e[48;2;" << to_string(red) << ";" << to_string(green) << ";" << to_string(blue) << "m";
    }
    // 38;2;⟨r⟩;⟨g⟩;⟨b⟩

    // Takes values between 0 and 5 inclusive
    static void setForegroundColour(int red, int green, int blue) {
        int colour_code = get256ColourCode(red, green, blue);
        cout << "\e[38;5;" << to_string(colour_code) << "m";
    }
    static void setBackgroundColour(int red, int green, int blue) {
        int colour_code = get256ColourCode(red, green, blue);
        cout << "\e[48;5;" << to_string(colour_code) << "m";
    }

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
        moveCursorUp(getHeight() - 1);
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

    static void setStdinFlag(int flag, bool state) {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        if (state) {
            term.c_lflag |= flag;

        } else {
            term.c_lflag &= ~flag;
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }

    static int get256ColourCode(int red, int green, int blue) {
        assert(0 <= red && red <= 5);
        assert(0 <= green && green <= 5);
        assert(0 <= blue && blue <= 5);
        int colour_code = 16 + 36 * red + 6 * green + blue;
        return colour_code;
    }
};
