#pragma once
#include "../components/widget.hpp"

struct TimeRuler : Widget
{
    int cells_between_seconds = 4;
    int cells_between_time_markers = 24;

    TimeRuler(){}

    void process(std::vector<KeyPress> &keyboard_input){}

    void render(Screen &screen)
    {
        DecoratedWindow border;
        Screen ruler = {screen.width - 2, 1};

        ScreenCellStyle time_marker_style, second_marker_style;
        time_marker_style.foreground_colour = Colour(1,1,1);
        second_marker_style.foreground_colour = Colour(0.5,1,1);
        for(int x = 0; x < screen.width; x++)
        {
            if(x % cells_between_time_markers == 0)
            {
                ruler.set_style(time_marker_style);
                ruler.draw(Point(x,0), to_string(x/cells_between_seconds));
            }
            else if(x % cells_between_seconds == 0)
            {
                ruler.set_style(second_marker_style);
                ruler.draw(Point(x,0),"|");
            }
        }
        border.render(screen);
        screen.draw(Point(1,1), ruler);
    }
};