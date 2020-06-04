#include "components/Colour.h"
#include "components/Style.h"
#include "components/TermControl.h"
#include "components/Window.h"
#include <codecvt>
#include <iostream>
#include <locale>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

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
        viewport = Screen(TermControl::getSize());
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
