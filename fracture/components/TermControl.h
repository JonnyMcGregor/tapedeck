#pragma once
#include "Colour.h"
#include "Point.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <sys/select.h>
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

enum struct ColourDepth {
    _4Bit,
    _8Bit,
    _24Bit,
};

struct TermControl {

    static ColourDepth getMaximumColourDepth() {
        return ColourDepth::_24Bit;
    }

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
        cout << "\e[3";
        cout << getColourCode(colour);
        cout << "m";
    }
    static void setBackgroundColour(Colour colour) {
        cout << "\e[4";
        cout << getColourCode(colour);
        cout << "m";
    }

    static string getColourCode(Colour colour) {
        ColourDepth colour_depth = getMaximumColourDepth();
        if (colour_depth == ColourDepth::_24Bit) {
            return get24BitColourCode(colour);
        } else if (colour_depth == ColourDepth::_8Bit) {
            return get8BitColourCode(colour);
        } else {
            return get4BitColourCode(colour);
        }
    }

    static string get24BitColourCode(Colour colour) {
        string true_colour_code = "8;2;";
        true_colour_code += to_string((int)(colour.red * 255)) + ";";
        true_colour_code += to_string((int)(colour.green * 255)) + ";";
        true_colour_code += to_string((int)(colour.blue * 255));
        return true_colour_code;
    }

    static string get8BitColourCode(Colour colour) {
        string _256_colour_code = "8;5;";
        int red = colour.red * 6;
        int green = colour.green * 6;
        int blue = colour.blue * 6;
        if (red > 5) red = 5;
        if (green > 5) green = 5;
        if (blue > 5) blue = 5;
        int colour_code = 16 + 36 * red + 6 * green + blue;
        _256_colour_code += to_string(colour_code);
        return _256_colour_code;
    }

    static string get4BitColourCode(Colour colour) {
        vector<Colour> colours = {
            Colour(0, 0, 0),
            Colour(1, 0, 0),
            Colour(0, 1, 0),
            Colour(1, 1, 0),
            Colour(0, 0, 1),
            Colour(1, 0, 1),
            Colour(0, 1, 1),
            Colour(1, 1, 1),
        };

        Colour closest_colour = getClosestColour(colour, colours);
        for (int i = 0; i < colours.size(); i++) {
            if (colours[i] == closest_colour) {
                return to_string(i);
            }
        }
        return ""; // unreachable
    }

    static Colour getClosestColour(Colour colour, vector<Colour> candidates) {
        Colour closest_colour;
        float shortest_hypotenuse = 2.0f;

        for (Colour candidate : candidates) {
            float hypotenuse = candidate.getHypotenuse(colour);
            if (hypotenuse < shortest_hypotenuse) {
                shortest_hypotenuse = hypotenuse;
                closest_colour = candidate;
            }
        }
        return closest_colour;
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
