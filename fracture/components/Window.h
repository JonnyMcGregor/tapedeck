#include "Border.h"
#include "Rect.h"
#include <string>

struct Window {
    Rect rect = {};
    string title = "";
    BorderStyle border_style;
    int z = 0;
    Screen screen = {};
    bool visible = true;

    Window(Rect rect, string title = "", BorderStyle border_style = BorderStyle::Plain, int z = 0) {
        this->title = title;
        this->rect = rect;
        this->border_style = border_style;
        this->z = z;
        resizeScreen();
    };

    void resizeScreen() {
        this->screen = Screen(rect.width, rect.height);
    }
};