#include "Fracture/Fracture.hpp" 
#include "Fracture/Widgets/Tapedeck.hpp"
#include <unistd.h>
#include <thread>
#include <map>
void runUIThread(Fracture* frac, Tapedeck* tapedeck)
{
    while(!tapedeck->closeUIThread)
    {
        frac->renderToViewport();
        usleep(1000000/60); // Hard sets the refresh rate of the screen
    }
    exit(0);
}

int main() 
{
    Tapedeck tapedeck;
    Fracture frac(tapedeck);

    std::thread ui_thread(runUIThread, &frac, &tapedeck);
    while(true)
    {
        frac.process();
    }
} 

