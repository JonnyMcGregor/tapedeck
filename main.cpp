#include <rtaudio/RtAudio.h>
#include <iostream>
#include <cstdlib>
#include <math.h>

double sampleRate = 44100.0, currentAngle = 0.0, angleDelta = 0.0; // [1]
double frequency = 110;

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
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    RtAudio::DeviceInfo info = dac.getDeviceInfo(parameters.deviceId);
    parameters.nChannels = info.outputChannels;
    parameters.firstChannel = 0;
    sampleRate = info.preferredSampleRate;
    unsigned int bufferFrames = 256; // 256 sample frames
    double data[2];

    //Start Streaming Audio
    try
    {
        dac.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
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
