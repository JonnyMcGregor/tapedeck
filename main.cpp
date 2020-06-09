#include "fracture/Fracture.h"
#include "seismic/components/Session.h"
#include "seismic/wave_file_generator/WaveFileGenerator.h"
#include <experimental/filesystem>
#include <rtaudio/RtAudio.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace experimental;

unsigned int sample_rate = 44100, buffer_size = 256; // [1]

WaveFileGenerator wav_gen;
RtAudio dac;
RtAudio::StreamParameters output_params, input_params;
RtAudio::DeviceInfo output_info, input_info;

int processAudioBlock(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                      double streamTime, RtAudioStreamStatus status, void *userData) {
    unsigned int sample_index, channel_index;
    double *out_buffer = (double *)outputBuffer;
    double *in_buffer = (double *)inputBuffer;
    Session *session = (Session *)userData;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    //if(RecordButtonIsPushed)
    //  session->play_state = Session::Play_State::Recording;
    //else if(PlayButtonIsPushed)
    //  session->play_state = Session::Play_State::Playing;

    session->processAudioBlock(in_buffer, out_buffer);

    return 0;
}

void initialiseAudioIO() {
    output_params.deviceId = dac.getDefaultOutputDevice();
    input_params.deviceId = dac.getDefaultInputDevice();

    output_info = dac.getDeviceInfo(output_params.deviceId);
    output_params.nChannels = output_info.outputChannels;
    output_params.firstChannel = 0;

    input_info = dac.getDeviceInfo(input_params.deviceId);
    input_params.nChannels = input_info.inputChannels;
    input_params.firstChannel = 0;

    sample_rate = input_info.preferredSampleRate;
}

void exportAllTracks(Session &session) {
    for (int i = 0; i < session.record_armed_tracks.size(); i++) {
        if (!filesystem::exists("exported_audio")) {
            filesystem::create_directory("exported_audio");
        }
        Clip clip = session.record_armed_tracks[i]->clips[0];
        std::ofstream audio_clip("exported_audio/" + clip.getName() + ".wav", std::ios::binary);
        wav_gen.openWaveFile(audio_clip);
        for (int sample = 0; sample < clip.getNumSamples(); sample++) {
            wav_gen.writeInputToFile(audio_clip, clip.getSample(sample));
        }
        wav_gen.closeWaveFile(audio_clip);
    }
}

void startRecording(Session &session) {
    if (session.tracks.size() > 0) {
        session.play_state = Session::Play_State::ToPlay;
        session.prepareAudio();
        dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                       sample_rate, &buffer_size, &processAudioBlock, &session);
        dac.startStream();
    }
}

int main() {
    //Setup Audio Devices and Parameters
    if (dac.getDeviceCount() < 1) {
        std::cout << "No audio devices found, exiting ...\n";
        exit(0);
    }
    initialiseAudioIO();
    wav_gen.initialise(sample_rate, 16, input_params.nChannels);
    Session session = {sample_rate, buffer_size};

    // Set up Fracture and windows
    Fracture frac = Fracture{};
    Window main_window = Window(
        ScreenSpaceRect(0, 0, -1, -1, frac.viewport),
        "AsciiDAW",
        Border(BorderStyle::Plain));
    frac.addWindow(main_window);

    // Main program loop
    string state = "main";
    bool error_state = false;
    bool export_menu = false;
    while (true) {

        main_window.screen.draw(Point(1, 0), "Welcome to AsciiDAW");
        main_window.screen.draw(Point(1, 1), "-------------------");

        KeyCombo key = frac.getKey();

        if (state == "main") {
            main_window.screen.draw(Point(1, 3), "Press T to create a track");
            main_window.screen.draw(Point(1, 4), "Press D to delete a track");
            if (session.tracks.size() > 0) {
                main_window.screen.draw(Point(1, 5), "Press R to start recording audio");
            }
            if (export_menu) {
                main_window.screen.draw(Point(1, 6), "Press E to export recordings to WAV files");
            }
            main_window.screen.draw(Point(1, 10), "Number of tracks: " + to_string(session.tracks.size()));
            main_window.screen.draw(Point(1, 11), "Number of channels: " + to_string(input_params.nChannels));

            // Create track
            if (key.keycode == KeyCode::K_T) {
                session.createTrack();
            }
            // Delete track
            if (key.keycode == KeyCode::K_D) {
                if (session.tracks.size() > 0) {
                    session.deleteTrack(session.tracks.size() - 1);
                }
            }
            // Start recording
            if (key.keycode == KeyCode::K_R) {
                try {
                    startRecording(session);
                } catch (RtAudioError &e) {
                    main_window.screen.draw(Point(1, 14), e.what());
                    error_state = true;
                }
                state = "recording";
            }
            // Export audio
            if (key.keycode == KeyCode::K_E) {
                exportAllTracks(session);
                main_window.screen.draw(Point(1, 14), "WAV files exported successfully");
                error_state = true;
                export_menu = false;
            }
        } else if (state == "recording") {
            int num_armed_tracks = session.record_armed_tracks.size();
            string record_message = "Recording to " + to_string(num_armed_tracks) + " armed track";
            if (num_armed_tracks != 1) record_message += "s";
            main_window.screen.draw(Point(1, 5), record_message);
            main_window.screen.draw(Point(1, 3), "Press R to stop recording audio");

            // Stop recording
            if (key.keycode == KeyCode::K_R) {
                try {
                    session.play_state = Session::Play_State::Stopping;
                    dac.stopStream();
                    session.play_state = Session::Play_State::Stopped;
                    dac.closeStream();

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
