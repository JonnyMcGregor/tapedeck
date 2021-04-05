#pragma once
#include "../components/widget.hpp"
#include "../../seismic/components/Playhead.hpp"
#include <memory>
struct PlayheadWidget : Widget
{
    PlayheadWidget(){}
    void process(std::vector<KeyPress> &keyboard_input){}
    void render(Screen& screen)
    {
        for(int y = 0; y < screen.height; y++)
        {
            screen.draw(Point(0,y), "|");
        }
    }
};
