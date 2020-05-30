#include "fracture/Fracture.h"
#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv) {
    Fracture frac = {};
    frac.viewport.draw(Point(10, 10), 'b');

    Window new_window = Window{
        Rect{0, 0, 10, 10},
        "Title",
        BorderStyle::Plain,
        0};

    frac.addWindow(new_window);

    Screen s = Screen(10, 10);
    s.draw(Point(1, 1), "Test stringasasd");

    frac.viewport.draw(Point(0, 0), s);

    frac.render();

    // TextFormat::setForegroundColour(Colour::Red);
    // colour.setBackgroundColour(Colour::Blue);

    return 0;
}