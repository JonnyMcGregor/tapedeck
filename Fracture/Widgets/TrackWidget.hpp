#pragma once
#include "../../Seismic/Components/Track.hpp"
#include "ClipWidget.hpp"
#include "DecoratedWindow.hpp"

struct TrackWidget : Widget {
    std::shared_ptr<Track> track;
    std::vector<std::unique_ptr<ClipWidget>> clipWidgets;
    std::shared_ptr<TimeRuler> timeRuler;
    TrackWidget(std::shared_ptr<Track> track, std::shared_ptr<TimeRuler> timeRuler) {
        this->track = track;
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.push_back(std::make_unique<ClipWidget>(track->clips[i], timeRuler, track->clipMetadata[i].startTime));
        }
        selectedWidgetStyle.foregroundColour = Colour(1, 1, 1);
        this->timeRuler = timeRuler;
    }

    void process(std::vector<KeyPress> &keyboardInput) {
        clipWidgets.clear();
        for (int i = 0; i < track->clips.size(); i++) {
            clipWidgets.push_back(std::make_unique<ClipWidget>(track->clips[i], timeRuler, track->clipMetadata[i].startTime));
        }
    }
    void render(Screen &screen) {
        ScreenCellStyle clip_style, track_style;
        clip_style.foregroundColour = Colour(1, 0, 0);
        track_style.foregroundColour = Colour(0, 0.5, 1);

        Screen track_bar_area = {screen.width * 0.1, screen.height};
        Screen track_properties_area = {track_bar_area.width - 2, track_bar_area.height - 2};
        Screen clip_area_border = {screen.width * 0.9, screen.height};
        Screen clip_data_area = {clip_area_border.width - 2, clip_area_border.height - 2};

        if (is_selected) {
            clip_area_border.set_style(selectedWidgetStyle);
            track_bar_area.set_style(selectedWidgetStyle);
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

        button_on_style.foregroundColour = Colour(1, 1, 1);
        button_on_style.backgroundColour = Colour(0.8, 0, 0);
        button_off_style.foregroundColour = Colour(1, 1, 1);
        button_off_style.backgroundColour = Colour(0, 0, 0);

        if (track->recordArmed) {
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
        if (clipWidgets.size() > 0) {
            for (int i = 0; i < clipWidgets.size(); i++) {
                if (clip_is_drawable(i)) {
                    Screen single_clip_screen = {calculate_clip_width(i), screen.height};
                    clipWidgets[i]->render(single_clip_screen);
                    screen.draw(Point(calculate_clip_start_cell(i), 0), single_clip_screen);
                }
            }
        }
    }

    bool clip_is_drawable(int clip_index) {
        if (timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples <=  track->clipMetadata[clip_index].startTime||
            track->clipMetadata[clip_index].startTime + track->clips[clip_index]->size() <= timeRuler->startTimeOnScreenInSamples) {
            return false;
        } else if(calculate_clip_width(clip_index) == 0) {
            return false;
        }
        else{
            return true;
        }
    }
    int calculate_clip_width(int clip_index) {
        return (calculate_clip_end_sample(clip_index) - calculate_clip_start_sample(clip_index)) / timeRuler->samplesPerCell();
    }

    int calculate_clip_start_sample(int clip_index) {
        return max((int)track->clipMetadata[clip_index].startTime, timeRuler->startTimeOnScreenInSamples);
    }
    int calculate_clip_end_sample(int clip_index) {
        return min((int)(track->clipMetadata[clip_index].startTime + track->clips[clip_index]->size()), timeRuler->startTimeOnScreenInSamples + timeRuler->windowSizeInSamples);
    }

      int calculate_clip_start_cell(int clip_index) {
        return max((int)(track->clipMetadata[clip_index].startTime - timeRuler->startTimeOnScreenInSamples) / timeRuler->samplesPerCell(), 0);
    }
};