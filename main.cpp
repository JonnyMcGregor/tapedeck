#include "fracture/Fracture.h"
#include "seismic/Seismic.hpp"
#include <stdio.h>
#include <unistd.h>

using namespace std;

int selected_track = 0;

void setSelectedTrack(KeyCombo key, Seismic &seismic) {
    if (key.keycode == KeyCode::K_O) {
        if (selected_track > 0) {
            selected_track--;
        }
    }
    if (key.keycode == KeyCode::K_P) {
        if (selected_track + 1 <= seismic.session->tracks.size()) {
            selected_track++;
        }
    }
}

string getStdOutFromCommand(string cmd) {

    string data;
    FILE *stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}

int main() {

    // Set up Fracture and windows
    Fracture frac = Fracture{};
    Window main_window = Window(
        ScreenSpaceRect(0, 0, -1, -1, frac.viewport),
        "AsciiDAW",
        Border(BorderStyle::Plain));
    frac.addWindow(main_window);
    std::unique_ptr<Seismic> seismic;
    std::shared_ptr<Session> session;
    // Main program loop
    string state = "start";
    bool error_state = false;
    string project_name = "";

    while (true) {
        main_window.screen.draw(Point(1, 0), "Welcome to AsciiDAW");
        main_window.screen.draw(Point(1, 1), "-------------------");

        if (state == "start") {
            main_window.screen.draw(Point(1, 2), "Press L to load an existing session");
            main_window.screen.draw(Point(1, 3), "Press C to create a new session");

            KeyCombo key = frac.getKey();

            if (key.keycode == KeyCode::K_L) {
                state = "load";
            } else if (key.keycode == KeyCode::K_C) {
                state = "input_name";
            }
        }

        if (state == "load") {
            filesystem::path xml_to_load = getStdOutFromCommand("zenity --file-selection");
            if (xml_to_load != "") {
                seismic = make_unique<Seismic>(true, "", xml_to_load);
                session = seismic->session;
                session->loadSessionFromXML(xml_to_load);
                state = "main";
            } else {
                state = "start";
            }
        }
        if (state == "input_name") {
            main_window.screen.draw(Point(1, 3), "Please Enter Name of Project: " + project_name);
            KeyCombo input = frac.getKey();
            if (input.keycode == KeyCode::K_E && input.modifier_keys == ModifierKey::Shift) {
                state = "new";
            } else if (!input.isNull()) {
                project_name += input.getChar();
            }
        }
        if (state == "new") {
            if (filesystem::exists(project_name)) {
                filesystem::remove_all(project_name);
            }
            seismic = make_unique<Seismic>(false, project_name, "");
            session = seismic->session;
            session->session_xml->createXMLDocument();
            state = "main";
        }
        KeyCombo key = frac.getKey();

        if (state == "main") {
            seismic->session->session_xml->refreshXMLDocument(session->playhead, session->tracks);
            main_window.screen.draw(Point(1, 3), "Press T to create a track");
            main_window.screen.draw(Point(1, 4), "Press D to delete a track");
            if (seismic->session->tracks.size() > 0) {
                main_window.screen.draw(Point(1, 5), "Press R to start recording audio");
            }

            main_window.screen.draw(Point(1, 7), "Number of tracks: " + to_string(session->tracks.size()));
            main_window.screen.draw(Point(1, 8), "Number of channels: " + to_string(seismic->params->num_input_channels));
            if (selected_track == 0) {
                main_window.screen.draw(Point(1, 10), "Selected Track: Null");
            } else {
                main_window.screen.draw(Point(1, 10), "Selected Track: " + session->tracks[selected_track - 1].name);
                if (seismic->session->tracks[selected_track - 1].record_armed) {
                    main_window.screen.draw(Point(25, 10), "(R)");
                }
                if (seismic->session->tracks[selected_track - 1].solo) {
                    main_window.screen.draw(Point(29, 10), "(S)");
                }
                if (seismic->session->tracks[selected_track - 1].mute) {
                    main_window.screen.draw(Point(33, 10), " (M)");
                }
            }

            main_window.screen.draw(Point(1, 12), "Current Time (s): " + to_string(session->getCurrentTimeInSeconds()));
            main_window.screen.draw(Point(1, 14), "Press A to arm/disarm tracks, S to solo, M to Mute");
            main_window.screen.draw(Point(1, 15), "Press K or L to move playhead");
            main_window.screen.draw(Point(1, 16), "Press O or P to change selected track");

            setSelectedTrack(key, *seismic.get());

            if (key.keycode == KeyCode::K_K) {
                session->playhead.movePlayhead(-0.5 * seismic->params->sample_rate);
            }
            if (key.keycode == KeyCode::K_L) {
                session->playhead.movePlayhead(0.5 * seismic->params->sample_rate);
            }
            if (key.keycode == KeyCode::K_A) {
                session->tracks[selected_track - 1].record_armed = !session->tracks[selected_track - 1].record_armed;
            }
            if (key.keycode == KeyCode::K_S) {
                session->tracks[selected_track - 1].solo = !session->tracks[selected_track - 1].solo;
            }
            if (key.keycode == KeyCode::K_M) {
                session->tracks[selected_track - 1].mute = !session->tracks[selected_track - 1].mute;
            }
            // Create track
            if (key.keycode == KeyCode::K_T) {
                session->createTrack();
                session->session_xml->refreshXMLDocument(session->playhead, session->tracks);
            }
            // Delete track
            if (key.keycode == KeyCode::K_D) {
                if (session->tracks.size() > 0) {
                    session->deleteTrack(seismic->session->tracks.size() - 1);
                    session->session_xml->refreshXMLDocument(session->playhead, session->tracks);
                }
            }
            // Start recording
            if (key.keycode == KeyCode::K_R) {
                try {
                    seismic->startAudioStream();
                } catch (RtAudioError &e) {
                    main_window.screen.draw(Point(1, 14), e.what());
                    error_state = true;
                }
                state = "recording";
            }
        } else if (state == "recording") {
            string record_message = "Recording to " + to_string(session->numRecordArmedTracks()) + " armed track";
            if (session->numRecordArmedTracks() != 1) record_message += "s";
            main_window.screen.draw(Point(1, 5), record_message);
            main_window.screen.draw(Point(1, 3), "Press R to stop recording audio");
            main_window.screen.draw(Point(1, 7), "Current Time (s): " + to_string(session->getCurrentTimeInSeconds()));
            // Stop recording
            if (key.keycode == KeyCode::K_R) {
                try {
                    seismic->stopAudioStream();
                } catch (RtAudioError &e) {
                    main_window.screen.draw(Point(1, 14), e.what());
                    error_state = true;
                }
                state = "main";
            }
        }

        frac.render();          // Draw everything to terminal
        usleep(1'000'000 / 60); // ~60 fps, plus frame processing time
        // Freeze UI to allow error messages to be read
        while (error_state) {
            // Continue if any key is pressed
            if (!frac.getKey().isNull()) error_state = false;
            usleep(1'000'000 / 60);
        }
        // Clear the main window to prevent lingering artifacts
        main_window.screen.clear();
    }
    return 0;
}
