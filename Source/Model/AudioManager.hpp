#pragma once
#include <JuceHeader.h>
#include "Components/Session.hpp"

//The back-end interface which manages the DSP and session structure of Tapedeck
class AudioManager : juce::AudioIODeviceCallback 
{
public:
    AudioManager(std::shared_ptr<juce::AudioDeviceManager> deviceManager, bool isLoadingFromXml, std::string projectName = "", filesystem::path xmlPath = "")
    {
        //Setup Audio Devices and Parameters
        deviceManager->initialiseWithDefaultDevices(1, 2);
        juce::AudioIODevice* currentDevice = deviceManager->getCurrentAudioDevice();
        params = std::make_unique<AudioParams>(currentDevice->getCurrentSampleRate(), 
                    currentDevice->getCurrentBufferSizeSamples(), 1, 2);
        this->deviceManager = deviceManager;
        //
        if (isLoadingFromXml) {
            this->sessionName = xmlPath.stem().string();
        } else {
            this->sessionName = projectName;
        }
        session = std::make_shared<Session>(this->sessionName, *params.get());
        //Setup Output Log
        audioDeviceLog = std::make_unique<juce::File>(juce::File::getCurrentWorkingDirectory().getFullPathName() + "\\" +  projectName + "_audio_device_log.txt");
        audioDeviceLog->appendText(juce::Time::getCurrentTime().toString(true, true, false, true) + " Audio Device Initialised:\n" + 
                        deviceManager->getCurrentAudioDevice()->getName() + "\n\tNum Input Channels: " + juce::String(params->numInputChannels) + 
                        "\n\tNum Output Channels: " + juce::String(params->numOutputChannels) + "\n\tSample rate: " + juce::String(params->sampleRate) + 
                        "\n\tBuffer Size: " + juce::String(params->bufferSize) + "\n==========\n");
    }

    ~AudioManager() {
    }

    void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples)
    {

        juce::AudioBuffer<float> outBuffer = {outputChannelData, numOutputChannels, numSamples};
        juce::AudioBuffer<float> inBuffer = {(float* const*)inputChannelData, numInputChannels, numSamples};

        session->processAudioBlock(inBuffer, outBuffer);
    }

    void audioDeviceAboutToStart(juce::AudioIODevice *device) {
        audioDeviceLog->appendText(juce::Time::getCurrentTime().toString(false, true, true, true) +  "\tAudio Device Started: " + device->getName() + "\n");
        
    }

    /** Called to indicate that the device has stopped. */
    void audioDeviceStopped(){
        audioDeviceLog->appendText(juce::Time::getCurrentTime().toString(false, true, true, true) +  "\tAudio Device Stopped\n");
    }

    void audioDeviceError (const juce::String& errorMessage)
    {
        audioDeviceLog->appendText(juce::Time::getCurrentTime().toString(false, true, true, true) + "\t" + errorMessage + "\n");
    }

    void startAudioStream() {
        if (session->tracks.size() == 0) { return; }      
        session->playState = Session::Play_State::ToPlay;
        session->prepareAudio(deviceManager->getCurrentAudioDevice()->getCurrentSampleRate(), deviceManager->getCurrentAudioDevice()->getCurrentBufferSizeSamples());
        //If tracks are armed, record audio
        if (session->numRecordArmedTracks() > 0) { session->playState = Session::Play_State::Recording; }
        //Else playback audio
        else { session->playState = Session::Play_State::Playing; }
        
        deviceManager->addAudioCallback(this);
    }

    void stopAudioStream() {
        deviceManager->removeAudioCallback(this);
        if (session->playState == Session::Play_State::Recording) {
            jassert(session->tracks.size() > 0);
            session->createFilesFromRecordedClips();
        }
        session->playState = Session::Play_State::Stopping;
        session->closeAudio();
        session->sessionXml->refreshXmlDocument(*session->playhead, session->tracks);
        session->playState = Session::Play_State::Stopped;
    }

    std::shared_ptr<Session> session;
    std::unique_ptr<AudioParams> params;
    bool isPlaying = false;

private:
    std::string sessionName;
    std::shared_ptr<juce::AudioDeviceManager> deviceManager;
    std::unique_ptr<juce::File> audioDeviceLog;
};

// void exportAllTracks() {
//     for (int i = 0; i < session->record_armed_tracks.size(); i++) {
//         assert(filesystem::exists(project_name + "/exported_audio"));
//         for (auto &clip : session->record_armed_tracks[i]->clips) {
//             std::ofstream audio_clip(project_name + "/exported_audio/" + clip.name + ".wav", std::ios::binary);
//             session->wav_gen.openWaveFile(audio_clip);
//             for (int sample = 0; sample < clip.size(); sample++) {
//                 session->wav_gen.writeInputToFile(audio_clip, clip.audio_data[sample].value);
//             }
//             session->wav_gen.closeWaveFile(audio_clip);
//         }
//     }
// }