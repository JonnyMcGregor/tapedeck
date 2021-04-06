#pragma once
#include "../../seismic/AudioManager.hpp"
#include "../components/point.hpp"
#include "../components/widget.hpp"
#include "decorated_window.hpp"
#include "playhead_widget.hpp"
#include "time_ruler.hpp"
#include "track_stack.hpp"
#include <iostream>
#include <optional>
#include <string>

struct TapeDeck : Widget {
    DecoratedWindow tapedeck_window;
    TrackStack track_stack;
    TimeRuler time_ruler;
    PlayheadWidget playhead_widget;
    TrackWidget *selected_widget = nullptr;
    std::string session_name;
    std::unique_ptr<AudioManager> audio_manager;
    std::shared_ptr<Session> session;
    bool is_playing = false;

    Clip clip1, clip2, clip3;

    TapeDeck() {
        session_name = "Test Session";
        if (filesystem::exists(session_name)) {
            filesystem::remove_all(session_name);
        }
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        this->tapedeck_window = dw;
        audio_manager = make_unique<AudioManager>(false, session_name, "");
        session = audio_manager->session;
        session->session_xml->create_xml_document();
        for(int i = 0; i < 4; i++)
        {
            create_track();
        }
    }
    
    //Manages all cases of keyboard input, needs a better method than nested if statements
    void process(std::vector<KeyPress> &keyboard_input) {
        session->session_xml->refresh_xml_document(*session->playhead, session->tracks);
        if (keyboard_input.size() > 0) {
            KeyPress last = keyboard_input.back();
            if (selected_widget != nullptr) {
                selected_widget->is_selected = false;
            }
            if (last == KeyPress(Key::K_Q, ModifierKey::Control))
                exit(1);
            if (last == KeyPress(Key::K_UpArrow)) {
                select_previous_track();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_DownArrow)) {
                select_next_track();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_LeftArrow)) {
                // scrub_backward
                session->playhead->movePlayhead(-0.5f);
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_RightArrow)) {
                // scrub_forward
                session->playhead->movePlayhead(0.5f);
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Equal)) {
                create_track();
                keyboard_input.pop_back();
            }

            if (last == KeyPress(Key::K_Minus)) {
                // remove_track
                remove_track();
                keyboard_input.pop_back();
            }

            if (last == KeyPress(Key::K_S)) {
                // toggle_track_solo
                selected_widget->track->solo = !selected_widget->track->solo;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_M)) {
                // toggle_track_mute
                selected_widget->track->mute = !selected_widget->track->mute;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_R)) {
                // toggle_track_armed
                selected_widget->track->record_armed = !selected_widget->track->record_armed;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Space)) {
                // play_pause
                track_stack.process(keyboard_input);
                if (is_playing) {
                    try {
                        audio_manager->stop_audio_stream();
                    } catch (RtAudioError &e) {
                        exit(1);
                    }
                    is_playing = false;
                } else {
                    try {
                        audio_manager->start_audio_stream();
                    } catch (RtAudioError &e) {
                        exit(1);
                    }
                    is_playing = true;
                }
                keyboard_input.pop_back();
            }

            if (selected_widget != nullptr) {
                selected_widget->is_selected = true;
            }
        }
    }
    void select_previous_track() {
        for (int i = 0; i < track_stack.sub_widgets.size(); i++) {
            if (track_stack.sub_widgets[i].get() == selected_widget) {
                if (i != 0) {
                    selected_widget = track_stack.sub_widgets[i - 1].get();
                } else {
                    selected_widget = track_stack.sub_widgets.back().get();
                    break;
                }
            }
        }
    }

    void select_next_track() {
        for (int i = 0; i < track_stack.sub_widgets.size(); i++) {
            if (track_stack.sub_widgets[i].get() == selected_widget) {
                if (i + 1 == track_stack.sub_widgets.size()) {
                    selected_widget = track_stack.sub_widgets.front().get();
                } else {
                    selected_widget = track_stack.sub_widgets[i + 1].get();
                    break;
                }
            }
        }
    }

    void create_track() {
        session->create_track();
        track_stack.create_track_sub_widget(session->tracks.back(), time_ruler.samples_per_cell(audio_manager->params->sample_rate));
        if (track_stack.sub_widgets.size() == 1) {
            selected_widget = track_stack.sub_widgets.back().get();
        }
    }

    void remove_track() {
        if (track_stack.sub_widgets.size() == 1) {
            selected_widget = nullptr;
        } else if (selected_widget == track_stack.sub_widgets.back().get() && track_stack.sub_widgets.size() > 1) {
            selected_widget = track_stack.sub_widgets[track_stack.sub_widgets.size() - 2].get();
        }
        track_stack.sub_widgets.pop_back();
        session->delete_track(session->tracks.size() - 1);
    }

    void render(Screen &screen) {
        Screen time_ruler_screen = {screen.width * 0.9, 3};
        Screen track_screen = {screen.width - 2, screen.height * 0.8 - 2};
        Screen playhead_screen = {1, track_screen.height};
        this->tapedeck_window.render(screen);
        time_ruler.render(time_ruler_screen);
        track_stack.render(track_screen);
        playhead_widget.render(playhead_screen);
        track_screen.draw(Point(playhead_x_position(track_screen), 0), playhead_screen);
        screen.draw(Point(screen.width * 0.1, screen.height * 0.2 - 1), time_ruler_screen);
        screen.draw(Point(1, screen.height * 0.2 + 1), track_screen);
    }

    int playhead_x_position(Screen track_screen) {
        int x_pos = track_screen.width * 0.1;
        x_pos += session->get_current_time_in_seconds() * time_ruler.cells_per_second;
        return x_pos;
    }
};
