#pragma once
#include "../Components/Widget.hpp"
#include "../../Seismic/Components/Playhead.hpp"
#include <memory>
struct PlayheadWidget : Widget
{
    PlayheadWidget(){}
    void process(std::vector<KeyPress> &keyboardInput){}
    void render(Screen& screen)
    {
        for(int y = 0; y < screen.height; y++)
        {
            screen.draw(Point(0,y), "|");
        }
    }
};
