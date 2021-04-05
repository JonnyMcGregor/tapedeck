#pragma once
#include "../../seismic/AudioManager.hpp"
#include "../components/point.hpp"
#include "../components/widget.hpp"
#include "track_stack.hpp"
#include "decorated_window.hpp"
#include <iostream>
#include <optional>
#include <string>

struct TapeDeck : Widget {
    DecoratedWindow tapedeck_window;
    TrackStack track_stack;
    Widget* selected_widget = nullptr;
    std::string session_name;
    std::unique_ptr<AudioManager> audio_manager;
    std::shared_ptr<Session> session;

    Clip clip1, clip2, clip3;

    TapeDeck() 
    {
        session_name = "Test Session";
        if (filesystem::exists(session_name)) 
        {
            filesystem::remove_all(session_name);
        }
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        this->tapedeck_window = dw;
        initialiseClip();
        audio_manager = make_unique<AudioManager>(false, session_name, "");
        session = audio_manager->session;
        session->session_xml->create_xml_document();
    }
    void initialiseClip()
    {
        float angle = 0;
        for(int i = 0; i < 48000; i++)
        {
            float current_sample = sin(angle);
            clip1.append_sample(current_sample);
            angle += (2*M_PI)/(24000);
        }
        angle = 0;
        for(int i = 0; i < 48000; i++)
        {
            float current_sample = sin(angle);
            clip2.append_sample(current_sample);
            angle += (2*M_PI)/(12000);
        }
        angle = 0;
        for(int i = 0; i < 48000; i++)
        {
            float current_sample = sin(angle);
            clip3.append_sample(current_sample);
            angle += (2*M_PI)/(6000);
        }
    }

    //Manages all cases of keyboard input, needs a better method than nested if statements
    void process(std::vector<KeyPress> &keyboard_input) 
    {
        if (keyboard_input.size() > 0) {
            KeyPress last = keyboard_input.back();
            if(selected_widget != nullptr)
            {
                selected_widget->is_focus = false;
            }
            if (last == KeyPress(Key::K_Q, ModifierKey::Control))
                exit(1);
            if (last == KeyPress(Key::K_UpArrow)) {
                // previous_track
                for(int i = 0; i < track_stack.sub_widgets.size(); i++)
                {
                    if(track_stack.sub_widgets[i].get() == selected_widget)
                    {
                        if(i != 0)
                        {
                            selected_widget = track_stack.sub_widgets[i - 1].get();
                        }
                        else
                        {
                            selected_widget = track_stack.sub_widgets.back().get();
                            break;
                        }
                    }
                }
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_DownArrow)) {
                for(int i = 0; i < track_stack.sub_widgets.size(); i++)
                {
                    if(track_stack.sub_widgets[i].get() == selected_widget)
                    {
                        if(i + 1 == track_stack.sub_widgets.size())
                        {
                            selected_widget = track_stack.sub_widgets.front().get();
                        }
                        else
                        {
                            selected_widget = track_stack.sub_widgets[i+1].get();
                            break;
                        }
                    }
                }
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_LeftArrow)) {
                // scrub_backward
            }
            if (last == KeyPress(Key::K_RightArrow)) {
                // scrub_forward
            }
            if (last == KeyPress(Key::K_Equal)) 
            {
                // create_track
                session->create_track();
                if(session->tracks.size() % 3 == 0)
                {
                    session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip1);
                }
                else if(session->tracks.size() % 2 == 0)
                {
                    session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip2);
                }
                else{
                    session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip3);
                }
                track_stack.create_track_sub_widget(session->tracks.back());
                selected_widget = track_stack.sub_widgets.back().get();
                keyboard_input.pop_back();
            }

            if (last == KeyPress(Key::K_Minus)) {
                // remove_track
                if(track_stack.sub_widgets.size() == 1)
                {
                    selected_widget = nullptr;
                }
                else if(selected_widget == track_stack.sub_widgets.back().get() && track_stack.sub_widgets.size() > 1)
                {
                    selected_widget = track_stack.sub_widgets[track_stack.sub_widgets.size() - 2].get();
                }
                track_stack.sub_widgets.pop_back();
                session->delete_track(session->tracks.size() - 1);
                keyboard_input.pop_back();
            }
            
            if (last == KeyPress(Key::K_S)) {
                // toggle_track_solo
            }
            if (last == KeyPress(Key::K_M)) {
                // toggle_track_mute
            }
            if (last == KeyPress(Key::K_R)) {
                // toggle_track_armed
            }
            if (last == KeyPress(Key::K_Space)) {
                // play_pause
            }
            
            if(selected_widget != nullptr)
            {
                selected_widget->is_focus = true;
            }
        }
        
    }
    void render(Screen &screen) {
        Screen track_screen = {screen.width - 2, screen.height * 0.8};
        this->tapedeck_window.render(screen);
        track_stack.render(track_screen);
        screen.draw(Point(1,screen.height * 0.2), track_screen);
    }
};
