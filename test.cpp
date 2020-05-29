#include "Fracture.h"
#include "TermColour.h"
#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv) {
    /*     TermColour colour;

    colour.setForegroundColour(colour.red);
    std::cout << "Test!\n";
    colour.setForegroundColour(colour.green);
    std::cout << "Test2!\n";
    colour.reset();
    std::cout << "Default\n";
 */
    /*     Fracture frac;
    std::cout << frac.getWidth() << "\n";
 */
    Screen screen = {10, 10};
    screen.draw(1, 1, 'a');
    screen.draw(4, 4, 'b');

    vector<string> result = screen.render();
    for (string row : result) {
        std::cout << "|" + row + "|"
                  << "\n";
    }

    return 0;
}