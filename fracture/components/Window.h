#include "Rect.h"
#include <string>
enum struct BorderStyle {
    Plain,
    Solid,
    Ascii,
    None,
};

struct Window {
    bool visible = true;
    string title;
    Rect rect;
    BorderStyle border_style;
    int z;

    Window(Rect rect, string title = NULL, BorderStyle border_style = BorderStyle::Plain, int z = 0) {
        this->title = title;
        this->rect = rect;
        this->border_style = border_style;
        this->z = z;
    };
};