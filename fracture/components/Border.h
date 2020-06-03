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
    TopLeft = 0,
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

    char32_t get(BorderElement element) {
        int i = static_cast<int>(element);
        return style_map.find(style)->second.at(i);
    }

    std::map<BorderStyle, std::u32string> style_map{
        {BorderStyle::Plain, U"┌┐└┘─│┼"},
        {BorderStyle::Twin, U"╔╗╚╝═║╬"},
        {BorderStyle::Solid, U"┏┓┗┛━┃╋"},
        {BorderStyle::Ascii, U"++++-|+"},
        {BorderStyle::None, U"       "},
        {BorderStyle::Minimal, U"┌┐└┘ │ "}};

    BorderStyle style;
};
