#include <iostream>
#include <string>
#include <map>
#include "TermColour.h"
#include "Fracture.h"

int main(int argc, char **argv)
{
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
    Screen screen(10, 10);
    screen.draw(1, 1, 'a');
    screen.draw(4, 4, 'b');

    vector<string> result = screen.render();
    for (string row : result)
    {
        std::cout << "|" + screen.render()[0] + "|"
                  << "\n";
    }
    for (int i = 0; i < 10; i++)
    {
        std::cout << screen.getRow(i) << "\n";
    }

    return 0;
}