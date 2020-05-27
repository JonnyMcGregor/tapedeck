#include <rtaudio/RtAudio.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <fstream>

double sampleRate = 44100.0, currentAngle = 0.0, angleDelta = 0.0; // [1]
double frequency = 110;
std::ofstream file01;
void updateAngleDelta()
{
    auto cyclesPerSample = frequency / sampleRate; // [2]
    angleDelta = cyclesPerSample * 2.0 * M_PI;     // [3]
}

// Two-channel sine wave generator.
int sin(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void *userData)
{
    unsigned int sampleIndex, channelIndex;
    double *buffer = (double *)outputBuffer;
    double *iBuffer = (double *)inputBuffer;

    double *currentBufferData = (double *)userData;
    if (status)
        std::cout << "Stream underflow detected!" << std::endl;
    // Write interleaved audio data.
    file01.open("input.txt");
    for (sampleIndex = 0; sampleIndex < nBufferFrames; sampleIndex++)
    {
        for (channelIndex = 0; channelIndex < 2; channelIndex++)
        {
            *buffer++ = currentBufferData[channelIndex];
            currentBufferData[channelIndex] = (float)std::sin(currentAngle);
            file01 << *iBuffer++ << std::endl;
            currentAngle += angleDelta;
            updateAngleDelta();
        }
    }
    file01.close();
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
    sampleRate = inputInfo.preferredSampleRate;
    unsigned int bufferFrames = 256; // 256 sample frames
    double data[2];
    //Start Streaming Audio
    try
    {
        dac.openStream(&outputParams, &inputParams, RTAUDIO_FLOAT64,
                       sampleRate, &bufferFrames, &sin, (void *)&data);
        dac.startStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
        exit(0);
    }

    char input;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
    std::cin.get(input);
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

    return 0;
}
