#include "Fracture.h"
#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv) {
    Fracture frac;

    TextFormat::setForegroundColour(Colour::red);
    // colour.setBackgroundColour(Colour::blue);
    std::cout << "Default\n";

    Screen screen = {10, 10};
    screen.draw(Point(1, 1), 'a');
    screen.draw(Point(4, 4), 'b');

    vector<string> result = screen.render();
    for (string row : result) {
        std::cout << "|" + row + "|"
                  << "\n";
    }

    return 0;
}