#include "fracture/Fracture.h"
#include <iostream>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv) {
    Fracture frac = {};
    // frac.viewport.draw(Point(10, 10), 'b');

    Window w1 = Window(Rect(5, 5, 30, 10), "Title", Border{BorderStyle::Plain}, 0);
    w1.screen.draw(Point(0, 0), "Character code:");
    frac.addWindow(w1);

    Window w2 = Window(ScreenSpaceRect(-20, 0, -1, -1, frac.viewport), "Title", Border{BorderStyle::Twin}, 0);
    w2.screen.draw(Point(0, 0), "A sidebar");
    frac.addWindow(w2);

    TermControl::setCursorVisible(false);
    int counter = 0;
    while (true) {
        TermControl::moveCursorToTopLeft();
        frac.render();
        usleep(100000);
        w1.screen.draw(Point(0, 1), "          ");
        w1.screen.draw(Point(0, 1), to_string(getch()));
        w1.screen.draw(Point(0, 4), to_string(counter));
        counter++;
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