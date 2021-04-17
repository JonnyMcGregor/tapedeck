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
    std::shared_ptr<TrackStack> track_stack;
    std::shared_ptr<TimeRuler> time_ruler;
    PlayheadWidget playhead_widget;
    TrackWidget *selected_track = nullptr;
    ClipWidget *selected_clip = nullptr;
    std::string session_name;
    std::unique_ptr<AudioManager> audio_manager;
    std::shared_ptr<Session> session;
    bool is_playing = false;
    bool close_ui_thread = false;

    TapeDeck() {
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        this->tapedeck_window = dw;
        initialise_session("test_session");
        selected_track = track_stack->sub_widgets.front().get();
        selected_track->is_selected = true;
    }

    void initialise_session(std::string session_name) {
        this->session_name = session_name;
        if (filesystem::exists(session_name)) {
            filesystem::remove_all(session_name);
        }
        audio_manager = make_unique<AudioManager>(false, session_name, "");
        session = audio_manager->session;
        session->session_xml->create_xml_document();
        time_ruler = make_shared<TimeRuler>(audio_manager->params->sample_rate);
        track_stack = make_shared<TrackStack>(time_ruler);
        for (int i = 0; i < 4; i++) {
            create_track();
        }
    }
    //Manages all cases of keyboard input, needs a better method than nested if statements
    void process(std::vector<KeyPress> &keyboard_input) {
        session->session_xml->refresh_xml_document(*session->playhead, session->tracks);
        if (keyboard_input.size() > 0) {
            KeyPress last = keyboard_input.back();
            if (selected_track != nullptr) {
                selected_track->is_selected = false;
            }
            if (selected_clip != nullptr) {
                selected_clip->is_selected = false;
            }
            if (last == KeyPress(Key::K_Q, ModifierKey::Control)) {
                close_ui_thread = true;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_UpArrow)) {
                select_previous_track();
                selected_clip = nullptr;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_DownArrow)) {
                select_next_track();
                selected_clip = nullptr;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_LeftArrow)) {
                if (selected_track->clip_widgets.size() > 0) {
                    select_previous_clip();
                }
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_RightArrow)) {
                if (selected_track->clip_widgets.size() > 0) {
                    select_next_clip();
                }
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Comma)) {
                // scrub_backward
                session->playhead->movePlayhead(-0.5f);
                retreat_clip_window();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Period)) {
                // scrub_forward
                session->playhead->movePlayhead(0.5f);
                advance_clip_window();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Comma, ModifierKey::Shift)) {
                // big_scrub_backward
                session->playhead->movePlayhead((-0.5f * 4.0f));
                retreat_clip_window();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Period, ModifierKey::Shift)) {
                // big_scrub_forward
                session->playhead->movePlayhead((0.5f * 4.0f));
                advance_clip_window();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_BracketOpen)) {
                // zoom_out
                time_ruler->cells_per_second *= 0.75;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_BracketClose)) {
                // zoom_in
                time_ruler->cells_per_second *= 1.25;

                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Equal)) {
                create_track();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Minus)) {
                remove_track();
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_S)) {
                // toggle_track_solo
                selected_track->track->solo = !selected_track->track->solo;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_M)) {
                // toggle_track_mute
                selected_track->track->mute = !selected_track->track->mute;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_R)) {
                // toggle_track_armed
                selected_track->track->record_armed = !selected_track->track->record_armed;
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Space)) {
                // play_pause
                if (is_playing) {
                    audio_manager->stop_audio_stream();
                    is_playing = false;
                    track_stack->process(keyboard_input);
                } else {
                    audio_manager->start_audio_stream();
                    is_playing = true;
                }
                keyboard_input.pop_back();
            }

            if (selected_track != nullptr) {
                selected_track->is_selected = true;
            } else if (track_stack->sub_widgets.size() > 0) {
                selected_track = track_stack->sub_widgets[0].get();
                selected_track->is_selected = true;
            }
            if (selected_clip != nullptr) {
                selected_clip->is_selected = true;
            }
        }
    }
    void select_previous_track() {
        for (int i = 0; i < track_stack->sub_widgets.size(); i++) {
            if (track_stack->sub_widgets[i].get() == selected_track) {
                if (i != 0) {
                    selected_track = track_stack->sub_widgets[i - 1].get();
                } else {
                    selected_track = track_stack->sub_widgets.back().get();
                    break;
                }
            }
        }
    }

    void select_next_track() {
        for (int i = 0; i < track_stack->sub_widgets.size(); i++) {
            if (track_stack->sub_widgets[i].get() == selected_track) {
                if (i + 1 == track_stack->sub_widgets.size()) {
                    selected_track = track_stack->sub_widgets.front().get();
                } else {
                    selected_track = track_stack->sub_widgets[i + 1].get();
                    break;
                }
            }
        }
    }
    void select_previous_clip() {
        for (int i = 0; i < selected_track->clip_widgets.size(); i++) {
            if (selected_track->clip_widgets[i].get() == selected_clip) {
                if (i != 0) {
                    selected_clip = selected_track->clip_widgets[i - 1].get();
                }
            }
        }
    }
    void select_next_clip() {
        for (int i = 0; i < selected_track->clip_widgets.size(); i++) {
            if (selected_track->clip_widgets[i].get() == selected_clip) {
                if (i + 1 != selected_track->clip_widgets.size()) {
                    selected_clip = selected_track->clip_widgets[i + 1].get();
                    break;
                }
            }
        }
        if (selected_clip == nullptr) {
            selected_clip = selected_track->clip_widgets[0].get();
        }
    }
    void advance_clip_window() {
        if (session->get_current_time_in_samples() > time_ruler->start_time_on_screen_in_samples + time_ruler->window_size_in_samples) {
            time_ruler->start_time_on_screen_in_samples = session->get_current_time_in_samples();
        }
    }
    void retreat_clip_window() {
        if (session->get_current_time_in_samples() < time_ruler->start_time_on_screen_in_samples) {
            time_ruler->start_time_on_screen_in_samples = session->get_current_time_in_samples() - time_ruler->window_size_in_samples;
            if (time_ruler->start_time_on_screen_in_samples < 0) {
                time_ruler->start_time_on_screen_in_samples = 0;
            }
        }
    }
    void create_track() {
        session->create_track();
        track_stack->create_track_sub_widget(session->tracks.back());
        if (track_stack->sub_widgets.size() == 1) {
            selected_track = track_stack->sub_widgets.back().get();
        }
    }

    void remove_track() {
        std::vector<std::unique_ptr<TrackWidget>>::iterator widget_iterator = track_stack->sub_widgets.begin();
        for (int i = 0; i < track_stack->sub_widgets.size(); i++) {
            if (selected_track == track_stack->sub_widgets[i].get()) {
                selected_track = nullptr;
                advance(widget_iterator, i);
                track_stack->sub_widgets.erase(widget_iterator);
                session->delete_track(i);
            }
        }
    }

    void render(Screen &screen) {
        Screen top_bar = {screen.width - 2, screen.height * 0.2 - 2};
        Screen time_ruler_screen = {screen.width * 0.9, 3};
        Screen track_screen = {screen.width - 2, screen.height * 0.8 - 2};
        Screen playhead_screen = {1, track_screen.height};

        this->tapedeck_window.render(screen);
        time_ruler->render(time_ruler_screen);
        track_stack->render(track_screen);
        playhead_widget.render(playhead_screen);

        top_bar.draw(Point(2, 1), "Key Commands:");
        top_bar.draw(Point(2, 3), "Create/Delete track    '='/'-'");
        top_bar.draw(Point(2, 4), "Navigate tracks        'Up'/'Down'");
        top_bar.draw(Point(2, 5), "Scrub through time     ','/'.'");
        top_bar.draw(Point(40, 3), "Record Arm Selected Track    'R'");
        top_bar.draw(Point(40, 4), "Solo Selected Track          'S'");
        top_bar.draw(Point(40, 5), "Mute Selected Track          'M'");
        top_bar.draw(Point(78, 3), "Playback/Record/Stop Audio    'Space'");
        top_bar.draw(Point(78, 4), "Zoom out/in                   '['/']'");
        top_bar.draw(Point(78, 4), "Quit Tapedeck                 'Ctrl-Q'");

        track_screen.draw(Point(playhead_x_position(track_screen), 0), playhead_screen);
        screen.draw(Point(1, 1), top_bar);
        screen.draw(Point(screen.width * 0.1, screen.height * 0.2 - 1), time_ruler_screen);
        screen.draw(Point(1, screen.height * 0.2 + 1), track_screen);
    }

    int playhead_x_position(Screen track_screen) {
        int x_pos = track_screen.width * 0.1;
        x_pos += (session->get_current_time_in_samples() - time_ruler->start_time_on_screen_in_samples) / time_ruler->samples_per_cell();
        if (x_pos >= track_screen.width - (track_screen.width * 0.9 * 0.5) && is_playing) {
            x_pos = track_screen.width - (track_screen.width * 0.9 * 0.5);
            update_clip_window();
        }
        return x_pos;
    }

    void update_clip_window() {
        time_ruler->start_time_on_screen_in_samples = session->get_current_time_in_samples() - time_ruler->window_size_in_samples / 2;
    }
};
