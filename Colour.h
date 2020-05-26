#include <string>

// typedef enum {
//         black = 30,
//         red,
//         green,
//         yellow,
//         blue,
//         magenta,
//         cyan,
//         white,
// } ColourCode;

class Colour
{
public:
    enum ColourEnum
    {
        black = 30,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
    };

    std::string getColourEscape(ColourEnum code)
    {
        return "\x1b[" + std::to_string(code) + "m";
    }
};