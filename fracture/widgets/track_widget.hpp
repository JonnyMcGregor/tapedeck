#pragma once
#include "../../seismic/components/Track.hpp"
#include "clip_widget.hpp"
#include "decorated_window.hpp"

struct TrackWidget : Widget {
    std::shared_ptr<Track> track;
    std::vector<std::unique_ptr<ClipWidget>> clip_widgets;
    std::shared_ptr<TimeRuler> time_ruler;
    TrackWidget(std::shared_ptr<Track> track, std::shared_ptr<TimeRuler> time_ruler) {
        this->track = track;
        for (int i = 0; i < track->clips.size(); i++) {
            clip_widgets.push_back(std::make_unique<ClipWidget>(track->clips[i], time_ruler, track->clip_metadata[i].start_time));
        }
        selected_widget_style.foreground_colour = Colour(1, 1, 1);
        this->time_ruler = time_ruler;
    }

    void process(std::vector<KeyPress> &keyboard_input) {
        clip_widgets.clear();
        for (int i = 0; i < track->clips.size(); i++) {
            clip_widgets.push_back(std::make_unique<ClipWidget>(track->clips[i], time_ruler, track->clip_metadata[i].start_time));
        }
    }
    void render(Screen &screen) {
        ScreenCellStyle clip_style, track_style;
        clip_style.foreground_colour = Colour(1, 0, 0);
        track_style.foreground_colour = Colour(0, 0.5, 1);

        Screen track_bar_area = {screen.width * 0.1, screen.height};
        Screen track_properties_area = {track_bar_area.width - 2, track_bar_area.height - 2};
        Screen clip_area_border = {screen.width * 0.9, screen.height};
        Screen clip_data_area = {clip_area_border.width - 2, clip_area_border.height - 2};

        if (is_selected) {
            clip_area_border.set_style(selected_widget_style);
            track_bar_area.set_style(selected_widget_style);
        } else {
            clip_area_border.set_style(track_style);
            track_bar_area.set_style(track_style);
        }

        DecoratedWindow clip_area;
        DecoratedWindow track_bar = {track->name};

        draw_toggle_buttons(track_properties_area);
        draw_clips(clip_data_area);

        track_bar.render(track_bar_area);
        clip_area.render(clip_area_border);

        track_bar_area.draw(Point(1, 1), track_properties_area);
        clip_area_border.draw(Point(1, 1), clip_data_area);
        screen.draw(Point(0, 0), track_bar_area);
        screen.draw(Point(screen.width * 0.1, 0), clip_area_border);
    }

    void draw_toggle_buttons(Screen &screen) {
        ScreenCellStyle button_on_style, button_off_style;

        button_on_style.foreground_colour = Colour(1, 1, 1);
        button_on_style.background_colour = Colour(0.8, 0, 0);
        button_off_style.foreground_colour = Colour(1, 1, 1);
        button_off_style.background_colour = Colour(0, 0, 0);

        if (track->record_armed) {
            Screen record_button = {3, 1};
            draw_button(record_button, button_on_style, "R");
            screen.draw(Point(screen.width / 4 - 1, screen.height / 2 - 1), record_button);
        } else {
            Screen record_button = {3, 1};
            draw_button(record_button, button_off_style, "R");
            screen.draw(Point(screen.width / 4 - 1, screen.height / 2 - 1), record_button);
        }

        if (track->solo) {
            Screen solo_button = {3, 1};
            draw_button(solo_button, button_on_style, "S");
            screen.draw(Point(screen.width * 2 / 4 - 1, screen.height / 2 - 1), solo_button);
        } else {
            Screen solo_button = {3, 1};
            draw_button(solo_button, button_off_style, "S");
            screen.draw(Point(screen.width * 2 / 4 - 1, screen.height / 2 - 1), solo_button);
        }

        if (track->mute) {
            Screen mute_button = {3, 1};
            draw_button(mute_button, button_on_style, "M");
            screen.draw(Point(screen.width * 3 / 4 - 1, screen.height / 2 - 1), mute_button);
        } else {
            Screen mute_button = {3, 1};
            draw_button(mute_button, button_off_style, "M");
            screen.draw(Point(screen.width * 3 / 4 - 1, screen.height / 2 - 1), mute_button);
        }
    }

    void draw_button(Screen &screen, ScreenCellStyle style, std::string character) {
        screen.set_style(style);
        screen.draw(Point(1, 0), character);
    }

    void draw_clips(Screen &screen) {
        if (clip_widgets.size() > 0) {
            for (int i = 0; i < clip_widgets.size(); i++) {
                if (clip_is_drawable(i)) {
                    Screen single_clip_screen = {calculate_clip_width(i), screen.height};
                    clip_widgets[i]->render(single_clip_screen);
                    screen.draw(Point(calculate_clip_start_cell(i), 0), single_clip_screen);
                }
            }
        }
    }

    bool clip_is_drawable(int clip_index) {
        if (track->clip_metadata[clip_index].start_time <= time_ruler->start_time_on_screen_in_samples + time_ruler->window_size_in_samples &&
            track->clip_metadata[clip_index].start_time + track->clips[clip_index]->size() > time_ruler->start_time_on_screen_in_samples) {
            return true;
        } else {
            return false;
        }
    }
    int calculate_clip_width(int clip_index) {
        return track->clips[clip_index]->size() / time_ruler->samples_per_cell();
    }

    int calculate_clip_start_cell(int clip_index) {
        return (track->clip_metadata[clip_index].start_time - time_ruler->start_time_on_screen_in_samples) / time_ruler->samples_per_cell();
    }
};