#include "fracture/fracture.hpp" 
#include "fracture/widgets/tapedeck.hpp"
#include <unistd.h>
#include <thread>
 

 
void load_ui(Fracture* frac, TapeDeck* tape_deck)
{
    while(!tape_deck->close_ui_thread)
    {
        frac->render_to_viewport();
        usleep(1000000/60);
    }
    exit(0);
}

int main() 
{
    TapeDeck tape_deck;
    Fracture frac(tape_deck);
    std::thread ui_thread(load_ui, &frac, &tape_deck);
    while(true)
    {
        frac.process();
    }
} 
