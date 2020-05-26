#include <iostream>
#include <string>
#include <map>
#include "Colour.h"

// default colours: "\x1b[0m"

// 30: black
// 31: red
// 32: green
// 33: yellow
// 34: blue
// 35: magenta
// 36: cyan
// 37: white.

//std::map<std::string, std::string> foreground_colour;



int main(int argc, char** argv) {

    Colour colour;

    std::cout << colour.getColourEscape(colour.red) << "Test!" << "\n";
    std::cout << colour.getColourEscape(colour.green) << "Test2!" << "\n";
    return 0;
}