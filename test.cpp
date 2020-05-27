#include <iostream>
#include <string>
#include <map>
#include "TermColour.h"
#include "Fracture.h"

int main(int argc, char **argv)
{
    TermColour colour;

    colour.setForegroundColour(colour.red);
    std::cout << "Test!\n";
    colour.setForegroundColour(colour.green);
    std::cout << "Test2!\n";
    colour.reset();
    std::cout << "Default\n";

    Fracture frac;
    std::cout << frac.getWidth();

    return 0;
}