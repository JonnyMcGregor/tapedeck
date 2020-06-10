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

    static void setForegroundColour(Colour colour) {
        cout << "\e[3" << getTrueColourCode(colour);
    }
    static void setBackgroundColour(Colour colour) {
        cout << "\e[4" << getTrueColourCode(colour);
    }
    static string getTrueColourCode(Colour colour) {
        string true_colour_code = "8;2;";
        true_colour_code += to_string((int)(colour.red * 255)) + ";";
        true_colour_code += to_string((int)(colour.green * 255)) + ";";
        true_colour_code += to_string((int)(colour.blue * 255)) + "m";
        return true_colour_code;
    }

    static string get256ColourCode(Colour colour) {
        // Convert float 0-1 to int 0-5
        string _256_colour_code = "8;5;";
        int red = colour.red * 256;
        int green = colour.green * 256;
        int blue = colour.blue * 256;
        int colour_code = 16 + 36 * red + 6 * green + blue;
        _256_colour_code += to_string(colour_code) + "m";
        return _256_colour_code;
    }

    static string get8ColourCode(Colour colour) {
        // Make function to calculate distance between two colours, using 3D hypotenuse.
        // Calculate which of eight is closest to given colour, using bash defaults (255/127/0 values).
        return "";
    }

    static void resetForegroundColour() {
        cout << "\e[39m";
    }
    static void resetBackgroundColour() {
        cout << "\e[49m";
    }

    static void resetColours() {
        resetForegroundColour();
        resetBackgroundColour();
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
};
