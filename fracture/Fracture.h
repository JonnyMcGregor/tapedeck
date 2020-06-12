#pragma once
#include "components/Colour.h"
#include "components/Key.h"
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
    vector<KeyCombo> key_buffer = {};

    Fracture() {
        viewport = Screen(TermControl::getSize());
        TermControl::setCursorVisible(false);
        TermControl::setEchoFlag(false);
    }

    void cleanup() {
        TermControl::setCursorVisible(true);
        TermControl::setEchoFlag(true);
        TermControl::setCanonicalFlag(true);
        TermControl::resetAll();
    }

    KeyCombo getKey(bool remove_from_buffer = true) {
        updateKeyBuffer();
        if (key_buffer.size() == 0) {
            return KeyCombo(KeyCode::Null);
        }
        KeyCombo key = key_buffer.back();
        if (remove_from_buffer) {
            key_buffer.pop_back();
        }
        return key;
    }

    void updateKeyBuffer() {
        while (TermControl::kbhit()) {
            key_buffer.push_back(getKeyCombo());
        }
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
