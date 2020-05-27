#include <rtaudio/RtAudio.h>
#include <cstdlib>
#include <math.h>
#include <fstream>
#include <iostream>
#include "endian.h"

int sampleRate = 44100; // [1]
double frequency = 110, currentAngle = 0.0, angleDelta = 0.0;

std::ofstream audioClip("input.wav", std::ios::binary);
void updateAngleDelta()
{
    auto cyclesPerSample = frequency / sampleRate; // [2]
    angleDelta = cyclesPerSample * 2.0 * M_PI;     // [3]
}

// Two-channel sine wave generator.
int processAudioBlock(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                      double streamTime, RtAudioStreamStatus status, void *userData)
{
    unsigned int sampleIndex, channelIndex;
    double *buffer = (double *)outputBuffer;
    double *iBuffer = (double *)inputBuffer;
    constexpr double max_amplitude = 32760; // "volume"
    double *currentBufferData = (double *)userData;

    if (status)
        std::cout << "Stream underflow detected!" << std::endl;

    // Write interleaved audio data.
    for (sampleIndex = 0; sampleIndex < nBufferFrames; sampleIndex++)
    {
        for (channelIndex = 0; channelIndex < 2; channelIndex++)
        {
            *buffer++ = currentBufferData[channelIndex];

            currentBufferData[channelIndex] = (float)std::sin(currentAngle);
            currentAngle += angleDelta;
            updateAngleDelta();
            //Write to wave file
            little_endian_io::write_word(audioClip, (int)(max_amplitude * *buffer), 2);

            //*iBuffer++;
        }
    }

    return 0;
}

int main()
{
    //Setup Audio Devices and Parameters
    RtAudio dac;
    if (dac.getDeviceCount() < 1)
    {
        std::cout << "\nNo audio devices found!\n";
        exit(0);
    }
    RtAudio::StreamParameters outputParams, inputParams;
    outputParams.deviceId = dac.getDefaultOutputDevice();
    inputParams.deviceId = dac.getDefaultInputDevice();
    RtAudio::DeviceInfo outputInfo = dac.getDeviceInfo(outputParams.deviceId);
    RtAudio::DeviceInfo inputInfo = dac.getDeviceInfo(inputParams.deviceId);
    outputParams.nChannels = outputInfo.outputChannels;
    inputParams.nChannels = inputInfo.inputChannels;
    outputParams.firstChannel = 0;
    inputParams.firstChannel = 0;
    sampleRate = outputInfo.preferredSampleRate;
    unsigned int bufferFrames = 256; // 256 sample frames
    double data[2];

    int bitsPerSample = 16;
    int byteRate = (sampleRate * bitsPerSample * outputParams.nChannels) / 8;
    int blockAlign = (bitsPerSample * outputParams.nChannels) / 8;

    // Write the file headers
    using namespace little_endian_io;
    audioClip << "RIFF----WAVEfmt ";                  // (chunk size to be filled in later)
    write_word(audioClip, 16, 4);                     // no extension data
    write_word(audioClip, 1, 2);                      // PCM - integer samples
    write_word(audioClip, outputParams.nChannels, 2); // two channels (stereo file)
    write_word(audioClip, sampleRate, 4);             // samples per second (Hz)
    write_word(audioClip, byteRate, 4);               // (Sample Rate * BitsPerSample * Channels) / 8
    write_word(audioClip, blockAlign, 2);             // data block size (size of two integer samples, one for each channel, in bytes)
    write_word(audioClip, bitsPerSample, 2);          // number of bits per sample (use a multiple of 8)

    // Write the data chunk header
    size_t data_chunk_pos = audioClip.tellp();
    audioClip << "data----"; // (chunk size to be filled in later)

    char input01;
    std::cout << "\nPress <enter> to play.\n";
    std::cin.get(input01);

    //Start Streaming Audio
    try
    {
        dac.openStream(&outputParams, &inputParams, RTAUDIO_FLOAT64,
                       sampleRate, &bufferFrames, &processAudioBlock, (void *)&data);
        dac.startStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
        exit(0);
    }
    char input02;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
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

    // (We'll need the final file size to fix the chunk sizes above)
    size_t file_length = audioClip.tellp();

    // Fix the data chunk header to contain the data size
    audioClip.seekp(data_chunk_pos + 4);
    write_word(audioClip, file_length - data_chunk_pos + 8);

    // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
    audioClip.seekp(0 + 4);
    write_word(audioClip, file_length - 8, 4);

    return 0;
}
