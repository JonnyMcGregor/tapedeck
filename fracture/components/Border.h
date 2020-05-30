#include <map>
#include <stdlib.h>
#include <string>
struct Border {
    Border(BorderStyle bs) {
        style = bs;
    }

    char get(BorderElement element) {
        int i = static_cast<int>(element);
        return style_map.find(style)->second.at(i);
    }

    std::map<BorderStyle, std::string> style_map{
        {BorderStyle::Normal, "┌┐└┘─│┼"},
        {BorderStyle::Twin, "╔╗╚╝═║╬"},
        {BorderStyle::Solid, "┏┓┗┛━┃╋"},
        {BorderStyle::Ascii, "++++-|+"},
        {BorderStyle::None, "       "},
        {BorderStyle::Minimal, "┌┐└┘ │ "}};

    BorderStyle style;
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

enum struct BorderStyle {
    Normal,
    Twin,
    Solid,
    Ascii,
    None,
    Minimal,
};