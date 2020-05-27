#include <string>
#include <iostream>

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

class TermColour
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

    void setForegroundColour(ColourEnum code)
    {
        std::cout << "\x1b[" + std::to_string(code) + "m";
    }

    void reset()
    {
        std::cout << "\033[0m";
    }
};