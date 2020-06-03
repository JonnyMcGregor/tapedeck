#include "Border.h"
#include "Rect.h"
#include <string>

struct Window {
    Rect rect = {};
    string title = "";
    Border border = {};
    int z = 0;
    Screen screen = {};
    bool visible = true;

    Window(Rect rect, string title = "", Border border = Border{BorderStyle::Plain}, int z = 0) {
        this->title = title;
        this->rect = rect;
        this->border = border;
        this->z = z;
        resizeScreen();
    };

    void resizeScreen() {
        this->screen = Screen(rect.width - 2, rect.height - 2);
    }

    Screen render() {
        Screen output_screen = Screen(rect.width, rect.height);

        // Draw border corners
        output_screen.draw(Point(0, 0), U'a');
        output_screen.draw(Point(rect.width - 1, 0), U'b');
        output_screen.draw(Point(0, rect.height - 1), U'c');
        output_screen.draw(Point(rect.width - 1, rect.height - 1), U'd');

        // Draw top and bottom edges
        for (int i = 1; i < rect.width - 1; i++) {
            output_screen.draw(Point(i, 0), U'-');
            output_screen.draw(Point(i, rect.height - 1), U'-');
        };
        // Draw left and right edges
        for (int i = 1; i < rect.height - 1; i++) {
            output_screen.draw(Point(0, i), U'|');
            output_screen.draw(Point(rect.width - 1, i), U'|');
        };

        output_screen.draw(Point(1, 1), screen);
        return output_screen;
    }
};