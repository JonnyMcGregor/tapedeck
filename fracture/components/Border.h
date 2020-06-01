#include <map>
#include <stdlib.h>
#include <string>

enum struct BorderStyle {
    Plain,
    Twin,
    Solid,
    Ascii,
    None,
    Minimal,
};

enum struct BorderElement {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Horizontal,
    Vertical,
    Intersection,
};

struct Border {

    Border() {}

    Border(BorderStyle bs) {
        style = bs;
    }

    char get(BorderElement element) {
        int i = static_cast<int>(element);
        return style_map.find(style)->second.at(i);
    }

    std::map<BorderStyle, std::string> style_map{
        {BorderStyle::Plain, "┌┐└┘─│┼"},
        {BorderStyle::Twin, "╔╗╚╝═║╬"},
        {BorderStyle::Solid, "┏┓┗┛━┃╋"},
        {BorderStyle::Ascii, "++++-|+"},
        {BorderStyle::None, "       "},
        {BorderStyle::Minimal, "┌┐└┘ │ "}};

    BorderStyle style;
};
