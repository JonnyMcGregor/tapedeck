#pragma once
#include "clip_widget.hpp"
#include "decorated_window.hpp"
#include "../../seismic/components/Track.hpp"

struct TrackWidget : Widget
{
    Track track;
    std::vector<std::unique_ptr<ClipWidget>> clip_widgets;
    TrackWidget(Track& track)
    {
        this->track = track;
        for(auto clip : track.clips)
        {
            clip_widgets.push_back(std::make_unique<ClipWidget>(clip));
        }
        selected_widget_style.foreground_colour = Colour(1,1,1);
    }

    void process(std::vector<KeyPress> &keyboard_input)
    {
        clip_widgets.push_back(std::make_unique<ClipWidget>(track.clips.back()));
    }

    void render(Screen& screen)
    {
        ScreenCellStyle clip_style, track_style, button_on_style, button_off_style;
        clip_style.foreground_colour = Colour(1,0,0);
        track_style.foreground_colour = Colour(0,0.5,1);
        button_on_style.foreground_colour = Colour(1,1,1);
        button_on_style.background_colour = Colour(0.8,0,0);
        button_off_style.foreground_colour = Colour(1,1,1);

        Screen track_bar = {screen.width * 0.1, screen.height};
        Screen track_properties_area = {track_bar.width - 2, track_bar.height - 2};
        Screen clip_area = {screen.width * 0.9, screen.height};
        Screen clip_data_area = {clip_area.width - 2, clip_area.height - 2};
        
        if(is_selected)
        {
            clip_area.set_style(selected_widget_style);
            track_bar.set_style(selected_widget_style);
        }
        else
        {
            clip_area.set_style(clip_style);
            track_bar.set_style(track_style);
        }
        

        DecoratedWindow clip_window = {"Clip Window"};
        DecoratedWindow track_window = {track.name};
        
        if(track.solo)
        {
            Screen solo_button = {3,1};
            draw_toggle_button(solo_button, button_on_style, "S");
            track_properties_area.draw(Point(track_properties_area.width/4 - 1, track_properties_area.height/2 - 1), solo_button);
        }
        else{
            Screen solo_button = {3,1};
            draw_toggle_button(solo_button, button_off_style, "S");
            track_properties_area.draw(Point(track_properties_area.width/4 - 1, track_properties_area.height/2 - 1), solo_button);
        }
        if(track.record_armed)
        {
            Screen record_button = {3,1};
            draw_toggle_button(record_button, button_on_style, "R");
            track_properties_area.draw(Point(track_properties_area.width * 2 / 4 - 1, track_properties_area.height/2 - 1), record_button);
        }
        else{
            Screen record_button = {3,1};
            draw_toggle_button(record_button, button_off_style, "R");
            track_properties_area.draw(Point(track_properties_area.width * 2 / 4 - 1, track_properties_area.height/2 - 1), record_button);
        }

        if(track.mute)
        {
            Screen mute_button = {3,1};
            draw_toggle_button(mute_button, button_on_style, "M");
            track_properties_area.draw(Point(track_properties_area.width * 3 / 4 - 1, track_properties_area.height/2 - 1), mute_button);
        }
        else
        {
            Screen mute_button = {3,1};
            draw_toggle_button(mute_button, button_off_style, "M");
            track_properties_area.draw(Point(track_properties_area.width * 3 / 4 - 1, track_properties_area.height/2 - 1), mute_button);
        }
        
        if(clip_widgets.size() > 0)
        {
            clip_widgets[0]->render(clip_data_area);
        }

        track_window.render(track_bar);
        clip_window.render(clip_area);

        track_bar.draw(Point(1,1), track_properties_area);
        clip_area.draw(Point(1,1), clip_data_area);
        screen.draw(Point(0, 0), track_bar);
        screen.draw(Point(screen.width * 0.1, 0), clip_area);
    }

    void draw_toggle_button(Screen &screen, ScreenCellStyle style, std::string character)
    {
        screen.set_style(style);
        screen.draw(Point(1,0), character);
    }
};