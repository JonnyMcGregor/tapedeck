#include "fracture/components/widget.hpp"
#include "fracture/fracture.hpp"
#include "fracture/widgets/decorated_window.hpp"
#include "seismic/Seismic.hpp"

struct TapeDeck : Widget {
    DecoratedWindow sub_widget;
    Seismic seismic;

    TapeDeck() {
        DecoratedWindow dw = DecoratedWindow("TAPEDECK");
        this->sub_widget = dw;
    }

    void process(std::vector<KeyPress> &keyboard_input) {
        if (keyboard_input.size() > 0) {
            KeyPress last = keyboard_input.back();
            switch (last) {
            case KeyPress(Key::K_Q, ModifierKey::Control):
                exit(1);
                break;
            case KeyPress(Key::K_UpArrow):
                // previous_track
            case KeyPress(Key::K_DownArrow):
                // next_track
            case KeyPress(Key::K_LeftArrow):
                // scrub_backward
            case KeyPress(Key::K_RightArrow):
                // scrub_forward
            case KeyPress(Key::K_Equal, ModifierKey::Shift):
                // create_track
            case KeyPress(Key::K_Minus):
                // remove_track
            case KeyPress(Key::K_S):
                // toggle_track_solo
            case KeyPress(Key::K_M):
                // toggle_track_mute
            case KeyPress(Key::K_R):
                // toggle_track_armed
            case KeyPress(Key::K_Space):
                // play_pause
            }
        }
    }

    void
    render(Screen &screen) {
        this->sub_widget.render(screen);
    };
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
