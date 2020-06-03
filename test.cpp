#include "fracture/Fracture.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
    Fracture frac = {};
    // frac.viewport.draw(Point(10, 10), 'b');

    Window new_window = Window{
        Rect{0, 0, 10, 10},
        "Title",
        BorderStyle::Plain,
        0};

    frac.addWindow(new_window);

    Window w = Window(Rect(0, 0, 30, 10), "Title", Border{BorderStyle::Plain}, 0);
    w.screen.draw(Point(0, 0), "Unicode support!!!");
    frac.viewport.draw(Point(5, 5), w.render());

    Window ww = Window(ScreenSpaceRect(-20, 0, -1, -1, frac.viewport), "Title", Border{BorderStyle::Twin}, 0);
    ww.screen.draw(Point(0, 0), "A sidebar");
    frac.viewport.draw(ww.rect.getOrigin(), ww.render());

    frac.render();

    // TextFormat::setForegroundColour(Colour::Red);
    // colour.setBackgroundColour(Colour::Blue);

    return 0;
}