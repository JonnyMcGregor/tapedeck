#include "AudioStructure/Session.h"
#include "KeyCodes.h"
#include "WavGen/WaveFileGenerator.h"
#include <rtaudio/RtAudio.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

unsigned int sample_rate = 44100, buffer_size = 256; // [1]

WaveFileGenerator wav_gen;
RtAudio dac;
RtAudio::StreamParameters output_params, input_params;
RtAudio::DeviceInfo output_info, input_info;

// Two-channel sine wave generator.
int processAudioBlock(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                      double streamTime, RtAudioStreamStatus status, void *userData) {
    unsigned int sample_index, channel_index;
    double *out_buffer = (double *)outputBuffer;
    double *in_buffer = (double *)inputBuffer;
    Session *session = (Session *)userData;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    session->processBlock(in_buffer, out_buffer);

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

int getch(void) {
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

void runTrackMenu(Session &session, char input, bool &exitCondition) {
    std::cout << "Press + to add or - to remove tracks. Press <<backspace>> to return to main menu."
              << "\n";

    input = getch();
    switch (input) {
    case Key_Plus:
        session.createTrack();
        std::cout << "Track Created. Current number of tracks: " << session.tracks.size() << "\n"
                  << "\n";
        break;
    case Key_Minus:
        if (session.tracks.size() > 0) {
            session.deleteTrack(session.tracks.size() - 1);
            std::cout << "Track Deleted. Current number of tracks: " << session.tracks.size() << "\n"
                      << "\n";
        }
        break;
    case Key_backspace:
        exitCondition = false;
        break;
    default:
        std::cout << "Invalid Character..." << std::endl;
    }
}
int main() {
    //Setup Audio Devices and Parameters
    if (dac.getDeviceCount() < 1) {
        std::cout << "\nNo audio devices found!\n";
        exit(0);
    }
    initialiseAudioIO();
    wav_gen.initialise(sample_rate, 16, input_params.nChannels);
    Session session = {sample_rate, buffer_size};
    std::cout << "\n"
              << "\n"
              << "Welcome to AsciiDAW"
              << "\n";
    while (true) {
        if (dac.isStreamOpen()) {
            std::cout << "Press <<space>> to stop audio" << endl;
            char input = getch();
            std::cout << input;
            if (input == Key_space) {
                try {
                    dac.stopStream();
                } catch (RtAudioError &e) {
                    e.printMessage();
                }
                dac.closeStream();

                bool exportMenu = true;
                while (exportMenu) {
                    std::cout << "Press E to export recordings as .wav files" << std::endl;
                    input = getch();
                    if (input == Key_e || input == Key_E) {
                        for (int i = 0; i < session.record_armed_tracks.size(); i++) {
                            Clip clip = session.record_armed_tracks[i]->clips[0];
                            std::cout << ".wav files created :)" << std::endl;
                            std::ofstream audio_clip("Exports/" + clip.getName() + ".wav", std::ios::binary);
                            wav_gen.openWaveFile(audio_clip);
                            for (int sample = 0; sample < clip.getNumSamples(); sample++) {
                                wav_gen.writeInputToFile(audio_clip, clip.getSample(sample));
                            }
                            wav_gen.closeWaveFile(audio_clip);
                        }

                        exportMenu = false;
                    } else
                        std::cout << "Invalid Character..." << std::endl;
                }
            } else
                std::cout << "Invalid Character..." << std::endl;
        } else {
            std::cout << "Press <<T>> to add or remove tracks"
                      << "\n"
                      << "Press <<Space>> to Play or Stop"
                      << "\n";

            bool trackMenu = true;
            char input = getch();
            std::cout << input;
            switch (input) {
            case Key_T:
            case Key_t:
                while (trackMenu) {
                    runTrackMenu(session, input, trackMenu);
                }
                break;
            case Key_space:
                if (session.tracks.size() > 0) {
                    session.prepareAudio();
                    try {
                        dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                                       sample_rate, &buffer_size, &processAudioBlock, &session);
                        dac.startStream();
                        std::cout << "Playing Audio, recording to " << session.record_armed_tracks.size() << " armed track/s" << std::endl;
                    } catch (RtAudioError &e) {
                        e.printMessage();
                        exit(0);
                    }
                }
                break;
            default:
                std::cout << (int)input;
            }
        }
    }
    return 0;
}
