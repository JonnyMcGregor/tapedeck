#include "fracture/components/widget.hpp"
#include "fracture/widgets/track_stack.hpp"
#include "fracture/fracture.hpp"
#include "seismic/AudioManager.hpp"
#include <rtaudio/RtAudio.h>

struct TapeDeck : Widget {
    DecoratedWindow tapedeck_window;
    TrackStack track_stack;
    
    std::string session_name;
    std::unique_ptr<AudioManager> audio_manager;
    std::shared_ptr<Session> session;

    Clip clip1, clip2, clip3;

    TapeDeck() {
        session_name = "Test Session";
         if (filesystem::exists(session_name)) {
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

    void process(std::vector<KeyPress> &keyboard_input) {
        if (keyboard_input.size() > 0) {
            KeyPress last = keyboard_input.back();

            if (last == KeyPress(Key::K_Q, ModifierKey::Control))
                exit(1);
            if (last == KeyPress(Key::K_UpArrow)) {
                // previous_track
            }
            if (last == KeyPress(Key::K_DownArrow)) {
                // next_track
            }
            if (last == KeyPress(Key::K_LeftArrow)) {
                // scrub_backward
            }
            if (last == KeyPress(Key::K_RightArrow)) {
                // scrub_forward
            }
            if (last == KeyPress(Key::K_Equal)) {
                // create_track
                session->create_track();
                // if(session->tracks.size() % 3 == 0)
                // {
                //     session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip1);
                // }
                // else if(session->tracks.size() % 2 == 0)
                // {
                //     session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip2);
                // }
                // else{
                //     session->tracks.back().create_clip(0, (session_name + "/recorded_audio/"), clip3);
                // }
                track_stack.create_track_sub_widget(session->tracks.back());
                keyboard_input.pop_back();
            }
            if (last == KeyPress(Key::K_Minus)) {
                // remove_track
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
        }
    }
    void render(Screen &screen) {
        Screen track_screen = {screen.width - 2, screen.height * 0.8};
        this->tapedeck_window.render(screen);
        track_stack.render(track_screen);
        screen.draw(Point(1,screen.height * 0.2), track_screen);
    }
};

int main() {  
    TapeDeck tapedeck = TapeDeck();
    Fracture frac = Fracture(tapedeck);
    while (true) {
        frac.process();
        frac.render_to_viewport();
        usleep(1'000'000 / 60); // ~60 fps, plus frame processing time
    }
}

// while (true) {

//     if (state == "load") {
//         filesystem::path xml_to_load = getStdOutFromCommand("zenity --file-selection");
//         if (xml_to_load != "") {
//             seismic = make_unique<Seismic>(true, "", xml_to_load);
//             session = seismic->session;
//             session->loadSessionFromXML(xml_to_load);
//             state = "main";
//         } else {
//             state = "start";
//         }
//     }
//     if (state == "new") {
//         if (filesystem::exists(project_name)) {
//             filesystem::remove_all(project_name);
//         }
//         seismic = make_unique<Seismic>(false, project_name, "");
//         session = seismic->session;
//         session->session_xml->createXMLDocument();
//         state = "main";
//     }

//     if (state == "main") {
//         seismic->session->session_xml->refreshXMLDocument(session->playhead, session->tracks);

//         if (key.keycode == KeyCode::K_R) {
//             try {
//                 seismic->startAudioStream();
//             } catch (RtAudioError &e) {
//                 main_window.screen.draw(Point(1, 14), e.what());
//                 error_state = true;
//             }
//             state = "recording";
//         }
//     } else if (state == "recording") {
//         // Stop recording
//         if (key.keycode == KeyCode::K_R) {
//             try {
//                 seismic->stopAudioStream();
//             } catch (RtAudioError &e) {
//                 main_window.screen.draw(Point(1, 14), e.what());
//                 error_state = true;
//             }
//             state = "main";
//         }
//     }
// }
// return 0;
// }