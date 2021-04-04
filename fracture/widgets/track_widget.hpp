#pragma once
#include "clip_widget.hpp"
#include "decorated_window.hpp"
#include "../../seismic/components/Track.hpp"

struct TrackWidget : Widget
{
    Track track;
    std::vector<ClipWidget> clip_widgets;
    TrackWidget(Track& track)
    {
        this->track = track;
        for(auto clip : track.clips)
        {
            clip_widgets.push_back(clip);
        }
    }

    void process(std::vector<KeyPress> &keyboard_input)
    {

    }

    void render(Screen& screen)
    {
        ScreenCellStyle clip_style, track_style;
        clip_style.foreground_colour = Colour(1,0,0);
        track_style.foreground_colour = Colour(0,0.5,1);

        Screen clip_screen = {screen.width * 0.9, screen.height};
        Screen track_bar = {screen.width * 0.1, screen.height};
        Screen clip_data_screen = {clip_screen.width - 2, clip_screen.height - 2};
        
        clip_screen.set_style(clip_style);
        track_bar.set_style(track_style);

        DecoratedWindow clip_window = {"Clip Window"};
        DecoratedWindow track_window = {track.name};
        clip_window.render(clip_screen);
        track_window.render(track_bar);
        clip_widgets[0].render(clip_data_screen);

        clip_screen.draw(Point(1,1), clip_data_screen);
        screen.draw(Point(0, 0), track_bar);
        screen.draw(Point(screen.width * 0.1, 0), clip_screen);

    }
};