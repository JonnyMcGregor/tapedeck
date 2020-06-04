#include "fracture/Fracture.h"
#include <iostream>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv) {
    Fracture frac = {};
    // frac.viewport.draw(Point(10, 10), 'b');

    Window w1 = Window(Rect(5, 5, 30, 10), "Title", Border{BorderStyle::Plain}, 0);
    w1.screen.draw(Point(0, 0), "Unicode support!!!");
    frac.addWindow(w1);

    Window w2 = Window(ScreenSpaceRect(-20, 0, -1, -1, frac.viewport), "Title", Border{BorderStyle::Twin}, 0);
    w2.screen.draw(Point(0, 0), "A sidebar");
    frac.addWindow(w2);

    TermControl::setCursorVisible(false);
    while (true) {
        w1.screen.draw(Point(0, 0), "AAA"); // to_string(getch());
        TermControl::moveCursorToTopLeft();
        frac.render();
        usleep(100000);
    }

    // TermControl::setForegroundColour(Colour::Cyan);
    // TermControl::clearScreen();
    // TermControl::moveCursorToTopLeft();
    // cout << "First\n";
    // cout << "Second\n";

    // TextFormat::setForegroundColour(Colour::Red);
    // colour.setBackgroundColour(Colour::Blue);

    return 0;
}