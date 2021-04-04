#include "fracture/components/widget.hpp"
#include "fracture/widgets/track_widget.hpp"
#include "fracture/fracture.hpp"
#include "fracture/widgets/vbox_layout.hpp"
#include "seismic/AudioManager.hpp"
#include <rtaudio/RtAudio.h>

struct TapeDeck : Widget {
    DecoratedWindow sub_widget;
    VBoxLayout clip_stack;
    std::string session_name;
    std::unique_ptr<AudioManager> audio_manager;
    Track track;
    Clip clip1, clip2, clip3;
    std::unique_ptr<ClipWidget> clip_widget_1;
    std::unique_ptr<ClipWidget> clip_widget_2;
    std::unique_ptr<ClipWidget> clip_widget_3;

    TapeDeck() {
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        
        this->sub_widget = dw;
        initialiseClip();
        track.clips.push_back(clip1);
        track.clips.push_back(clip2);
        track.clips.push_back(clip3);
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
            if (last == KeyPress(Key::K_Equal, ModifierKey::Shift)) {
                // create_track
            }
            if (last == KeyPress(Key::K_Minus)) {
                // remove_track
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

        //this->sub_widget.render(screen);
        // clip_stack.sub_widgets.clear();
        // clip_widget_1 = std::make_unique<ClipWidget>(clip1);
        // clip_widget_2 = std::make_unique<ClipWidget>(clip2);
        // clip_widget_3 = std::make_unique<ClipWidget>(clip3);
        // clip_stack.add_sub_widget(*clip_widget_1);
        // clip_stack.add_sub_widget(*clip_widget_2);
        // clip_stack.add_sub_widget(*clip_widget_3);
        // clip_stack.render(screen);
        TrackWidget track_widget = {track};
        Screen track_screen = {screen.width, screen.height*0.25};
        track_widget.render(track_screen);
        screen.draw(Point(0,0), track_screen);
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
