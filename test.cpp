#include <iostream>
#include <string>
#include <map>
#include "TermColour.h"

int main(int argc, char **argv)
{

    TermColour colour;

    colour.setForegroundColour(colour.red);
    std::cout << "Test!\n";
    colour.setForegroundColour(colour.green);
    std::cout << "Test2!\n";
    return 0;
}