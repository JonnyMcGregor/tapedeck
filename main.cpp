#pragma once

#include "AudioStructure/Session.h"
#include "WavGen/WaveFileGenerator.h"
#include <rtaudio/RtAudio.h>

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

int main() {
    //Setup Audio Devices and Parameters
    if (dac.getDeviceCount() < 1) {
        std::cout << "\nNo audio devices found!\n";
        exit(0);
    }
    initialiseAudioIO();
    wav_gen.initialise(sample_rate, 16, input_params.nChannels);
    Session session = {sample_rate, buffer_size};

    //Start Streaming Audio
    char input01;
    std::cout << "\nPress <enter> to create a track.\n";
    std::cin.get(input01);
    session.createTrack();

    std::cout << "\nPress <enter> to record a clip.\n";
    std::cin.get(input01);
    session.prepareAudio();
    try {
        dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                       sample_rate, &buffer_size, &processAudioBlock, &session);
        dac.startStream();
    } catch (RtAudioError &e) {
        e.printMessage();
        exit(0);
    }

    std::cout << "\nRecording ... Press <enter> to stop recording clip.\n";
    std::cin.get(input01);

    try {
        // Stop the stream
        dac.stopStream();
    } catch (RtAudioError &e) {
        e.printMessage();
    }

    if (dac.isStreamOpen())
        dac.closeStream();

    std::cout << "\nRecording ... Press <enter> to export clip as .wav.\n";
    std::cin.get(input01);
    Clip clip = session.tracks[0].clips[0];
    std::ofstream audio_clip(clip.getName() + ".wav", std::ios::binary);
    wav_gen.openWaveFile(audio_clip);
    for (int sample = 0; sample < clip.getNumSamples(); sample++) {
        wav_gen.writeInputToFile(audio_clip, clip.getSample(sample));
    }
    wav_gen.closeWaveFile(audio_clip);

    return 0;
}
