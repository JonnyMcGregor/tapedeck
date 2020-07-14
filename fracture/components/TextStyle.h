#pragma once

namespace TextDecoration {
    enum {
        Italic = 1,
        Reverse = 2,
        Strikethrough = 4,
    };
};

enum struct TextUnderline {
    None,
    Single,
    Double,
    Curly,
};

enum struct TextWeight {
    Dim,
    Normal,
    Bold,
};

struct TextStyle {
    Colour foreground_colour = Colour{0.8, 0.8, 0.8};
    Colour background_colour = Colour{0, 0, 0};
    Colour underline_colour = Colour{};
    TextWeight weight = TextWeight::Normal;
    TextUnderline underline = TextUnderline::None;
    int decoration = 0;
};