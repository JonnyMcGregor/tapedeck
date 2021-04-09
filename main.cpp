#include "fracture/fracture.hpp" 
#include "fracture/widgets/tapedeck.hpp"
#include <unistd.h>
int main()
{
    TapeDeck tape_deck;
    Fracture frac(tape_deck);

    while(true)
    {
        frac.process();
        frac.render_to_viewport();
        usleep(1000000/60);
    }
} 