#include "fracture/Fracture.h"
#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv) {
    Fracture frac;

    Window new_window = Window{
        Rect{0, 0, 10, 10},
        "Title",
        BorderStyle::Plain,
        0};

    frac.addWindow(new_window);

    TextFormat::setForegroundColour(Colour::Red);
    // colour.setBackgroundColour(Colour::Blue);
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