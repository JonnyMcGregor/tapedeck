#pragma once
#include "components/Colour.h"
#include "components/TermControl.h"
#include "components/Window.h"
#include <codecvt>
#include <iostream>
#include <locale>
using namespace std;

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
        TermControl::moveCursorToTopLeft();

        wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
        vector<u32string> rendered_viewport = viewport.render();

        for (int y = 0; y < viewport.height; y++) {
            for (int x = 0; x < viewport.width; x++) {
                cout << converter.to_bytes(viewport.content[y][x].character);
            }
        }
    }
};
