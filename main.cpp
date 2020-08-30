#include "fracture/Fracture.h"
#include "seismic/Seismic.h"
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

    // Main program loop
    string state = "start";
    bool error_state = false;
    bool export_menu = false;
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
                state = "new";
            }
        }

        if (state == "load") {
            filesystem::path xml_to_load = getStdOutFromCommand("zenity --file-selection");
            seismic = make_unique<Seismic>(true, "", xml_to_load);
            seismic->seismic_xml->loadSessionFromXML();
            seismic->session->loadAllAudioClips();
            state = "main";
        }
        if (state == "new") {
            if (filesystem::exists("seismic_test_project")) {
                filesystem::remove_all("seismic_test_project");
            }
            seismic = make_unique<Seismic>(false, "seismic_test_project", "");
            seismic->seismic_xml->createXMLDocument();
            state = "main";
        }
        KeyCombo key = frac.getKey();

        if (state == "main") {
            seismic->seismic_xml->refreshXMLDocument();
            main_window.screen.draw(Point(1, 3), "Press T to create a track");
            main_window.screen.draw(Point(1, 4), "Press D to delete a track");
            if (seismic->session->tracks.size() > 0) {
                main_window.screen.draw(Point(1, 5), "Press R to start recording audio");
            }
            if (export_menu) {
                main_window.screen.draw(Point(1, 6), "Press E to export recordings to WAV files");
            }
            main_window.screen.draw(Point(1, 7), "Press A to arm/disarm tracks");
            main_window.screen.draw(Point(1, 10), "Number of tracks: " + to_string(seismic->session->tracks.size()));
            main_window.screen.draw(Point(1, 11), "Number of channels: " + to_string(seismic->params->num_input_channels));
            if (selected_track == 0) {
                main_window.screen.draw(Point(1, 13), "Selected Track: Null");

            } else if (seismic->session->tracks[selected_track - 1].is_record_enabled) {
                main_window.screen.draw(Point(1, 13), "Selected Track: " + seismic->session->tracks[selected_track - 1].getName() + " (R)");
            } else {
                main_window.screen.draw(Point(1, 13), "Selected Track: " + seismic->session->tracks[selected_track - 1].getName());
            }
            main_window.screen.draw(Point(1, 15), "Current Time (s): " + to_string(seismic->session->getCurrentTimeInSeconds()));

            setSelectedTrack(key, *seismic.get());

            if (key.keycode == KeyCode::K_K) {
                seismic->session->movePlayhead(-0.5 * seismic->params->sample_rate);
            }
            if (key.keycode == KeyCode::K_L) {
                seismic->session->movePlayhead(0.5 * seismic->params->sample_rate);
            }
            if (key.keycode == KeyCode::K_A) {
                seismic->session->tracks[selected_track - 1].is_record_enabled = !seismic->session->tracks[selected_track - 1].is_record_enabled;
            }
            // Create track
            if (key.keycode == KeyCode::K_T) {
                seismic->session->createTrack();
                seismic->seismic_xml->refreshXMLDocument();
            }
            // Delete track
            if (key.keycode == KeyCode::K_D) {
                if (seismic->session->tracks.size() > 0) {
                    seismic->session->deleteTrack(seismic->session->tracks.size() - 1);
                    seismic->seismic_xml->refreshXMLDocument();
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
            // Export audio
            if (key.keycode == KeyCode::K_E) {
                seismic->exportAllTracks();
                main_window.screen.draw(Point(1, 14), "WAV files exported successfully");
                error_state = true;
                export_menu = false;
            }
        } else if (state == "recording") {
            int num_armed_tracks = seismic->session->record_armed_tracks.size();
            string record_message = "Recording to " + to_string(num_armed_tracks) + " armed track";
            if (num_armed_tracks != 1) record_message += "s";
            main_window.screen.draw(Point(1, 5), record_message);
            main_window.screen.draw(Point(1, 3), "Press R to stop recording audio");
            main_window.screen.draw(Point(1, 7), "Current Time (s): " + to_string(seismic->session->getCurrentTimeInSeconds()));
            // Stop recording
            if (key.keycode == KeyCode::K_R) {
                try {
                    seismic->stopAudioStream();
                } catch (RtAudioError &e) {
                    main_window.screen.draw(Point(1, 14), e.what());
                    error_state = true;
                }
                state = "main";
                export_menu = true;
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
