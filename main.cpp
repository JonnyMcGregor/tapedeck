#include "Fracture/Fracture.hpp" 
#include "Fracture/Widgets/Tapedeck.hpp"
#include <unistd.h>
#include <thread>

void runUIThread(Fracture* frac, TapeDeck* tapeDeck)
{
    while(!tapeDeck->closeUIThread)
    {
        frac->renderToViewport();
        usleep(1000000/60);
    }
    exit(0);
}

int main() 
{
    TapeDeck tapeDeck;
    Fracture frac(tapeDeck);
    std::thread ui_thread(runUIThread, &frac, &tapeDeck);
    while(true)
    {
        frac.process();
    }
} 

