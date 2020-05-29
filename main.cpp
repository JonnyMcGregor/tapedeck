#include <rtaudio/RtAudio.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include "WaveFileGenerator.h"
#include "Track.h"
unsigned int sample_rate = 44100, buffer_size = 256; // [1]
double frequency = 110, current_angle = 0.0, angle_delta = 0.0;

WaveFileGenerator wav_gen;
//std::ofstream audio_clip("input.wav", std::ios::binary);
RtAudio dac;
RtAudio::StreamParameters output_params, input_params;
RtAudio::DeviceInfo output_info, input_info;

Track track01;
int current_clip = 0;

void updateAngleDelta()
{
    auto cycles_per_sample = frequency / sample_rate; // [2]
    angle_delta = cycles_per_sample * 2.0 * M_PI;     // [3]
}

// Two-channel sine wave generator.
int processAudioBlock(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                      double streamTime, RtAudioStreamStatus status, void *userData)
{
    unsigned int sample_index, channel_index;
    double *out_buffer = (double *)outputBuffer;
    double *in_buffer = (double *)inputBuffer;
    double *current_buffer_data = (double *)userData;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    // Write interleaved audio data.
    for (sample_index = 0; sample_index < nBufferFrames; sample_index++)
    {
        for (channel_index = 0; channel_index < 2; channel_index++, *out_buffer++, *in_buffer++)
        {
            //*out_buffer = current_buffer_data[channel_index];

            current_buffer_data[channel_index] = (float)std::sin(current_angle);
            current_angle += angle_delta;
            updateAngleDelta();
            //Write to wave file
            wav_gen.writeInputToFile(track01.clips[current_clip], *in_buffer);
        }
    }
    return 0;
}

void initialiseAudioIO()
{
    output_params.deviceId = dac.getDefaultOutputDevice();
    input_params.deviceId = dac.getDefaultInputDevice();
    output_info = dac.getDeviceInfo(output_params.deviceId);
    input_info = dac.getDeviceInfo(input_params.deviceId);
    output_params.nChannels = output_info.outputChannels;
    input_params.nChannels = input_info.inputChannels;
    output_params.firstChannel = 0;
    input_params.firstChannel = 0;
    sample_rate = output_info.preferredSampleRate;
}

int main()
{
    //Setup Audio Devices and Parameters
    if (dac.getDeviceCount() < 1)
    {
        std::cout << "\nNo audio devices found!\n";
        exit(0);
    }
    initialiseAudioIO();
    double data[2];
    wav_gen.initialise(sample_rate, 16, output_params.nChannels);

    //Start Streaming Audio
    while (true)
    {
        char input01;
        std::cout << "\nEnter <p> to record.\n";
        std::cin.get(input01);

        wav_gen.openWaveFile(track01.clips[current_clip]);
        try
        {
            dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                           sample_rate, &buffer_size, &processAudioBlock, (void *)&data);
            dac.startStream();
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
            exit(0);
        }

        char input02;
        std::cout << "\nRecording ... Enter <s> to stop.\n";
        std::cin.get(input02);

        try
        {
            // Stop the stream
            dac.stopStream();
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
        }

        if (dac.isStreamOpen())
            dac.closeStream();

        wav_gen.closeWaveFile(track01.clips[current_clip]);
        current_clip++;
    }

    return 0;
}
