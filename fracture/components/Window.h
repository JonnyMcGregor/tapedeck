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

        string top_border = "";
        top_border += border.get(BorderElement::TopLeft);
        string bottom_border = "";
        bottom_border += border.get(BorderElement::BottomLeft);
        for (int i = 0; i < rect.width - 2; i++) {
            top_border += border.get(BorderElement::Horizontal);
            bottom_border += border.get(BorderElement::Horizontal);
        };
        top_border += border.get(BorderElement::TopRight);
        bottom_border += border.get(BorderElement::BottomRight);

        for (int i = 1; i < rect.height - 1; i++) {
            output_screen.draw(Point(0, i), border.get(BorderElement::Vertical));
            output_screen.draw(Point(rect.width - 1, i), border.get(BorderElement::Vertical));
        };

        output_screen.draw(Point(0, 0), top_border);
        output_screen.draw(Point(1, 1), screen);
        output_screen.draw(Point(0, rect.height - 1), bottom_border);
        return output_screen;
    }
};