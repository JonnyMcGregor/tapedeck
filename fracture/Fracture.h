#include "components/Colour.h"
#include "components/Style.h"
#include "components/Window.h"
#include <codecvt>
#include <iostream>
#include <locale>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <vector>

using namespace std;

struct TextFormat {
    static void setForegroundColour(Colour colour) {
        std::cout << "\033[3" + std::to_string(getColourValue(colour)) + "m";
    }
    static void setBackgroundColour(Colour colour) {
        std::cout << "\033[4" + std::to_string(getColourValue(colour)) + "m";
    }
    static void resetColours() {
        setForegroundColour(Colour::Reset);
        setBackgroundColour(Colour::Reset);
    }
    static void resetAll() {
        std::cout << "\033[0m";
    }

private:
    static int getColourValue(Colour colour) {
        return static_cast<int>(colour);
    }
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

/* One Fracture class should exist in a program. It holds the viewport Screen,
   as well as a vector of Windows. The viewport and all windows will resize
   every time the terminal is resized. */
struct Fracture {
    Screen viewport = {0, 0};
    vector<Window> windows = {};
    Fracture() {
        viewport = Screen(TermSize::getSize());
    }

    void addWindow(Window &window) {
        windows.push_back(window);
    }

    void flattenWindowsToViewport() {
        for (Window w : windows) {
            viewport.draw(w.rect.getOrigin(), w.render());
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
