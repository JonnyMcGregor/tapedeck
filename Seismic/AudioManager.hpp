#include "Components/Session.hpp"
#include "juce_modules/juce_audio_devices/juce_audio_devices.h"
#include <rtaudio/RtAudio.h>

//The back-end interface which manages the DSP and session structure of TapeDeck
class AudioManager : juce::AudioIODeviceCallback {
public:
    AudioManager(bool isLoadingFromXml, std::string projectName = "", filesystem::path xmlPath = "") {
        //Setup Audio Devices and Parameters
        deviceManager.initialiseWithDefaultDevices(1, 2);
        juce::AudioIODevice* currentDevice = deviceManager.getCurrentAudioDevice();
        params = std::make_unique<AudioParams>(currentDevice->getCurrentSampleRate(), 
                    currentDevice->getCurrentBufferSizeSamples(), 1, 2);
        if (isLoadingFromXml) {
            this->sessionName = xmlPath.stem();
        } else {
            this->sessionName = projectName;
        }
        session = std::make_shared<Session>(this->sessionName, *params.get());
    }

    ~AudioManager() {
    }

    void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples) {

        Buffer outBuffer = {outputChannelData};
        Buffer inBuffer = {inputChannelData};
        
        //If tracks are armed, record audio
        if (session->numRecordArmedTracks() > 0)
            session->playState = Session::Play_State::Recording;
        //Else playback audio
        else
            session->playState = Session::Play_State::Playing;

        session->processAudioBlock(inBuffer, outBuffer);
    }

    void audioDeviceAboutToStart(juce::AudioIODevice *device) {
    }

    /** Called to indicate that the device has stopped. */
    void audioDeviceStopped(){
    }

    void startAudioStream() {
        if (session->tracks.size() == 0)
            return;
        session->playState = Session::Play_State::ToPlay;
        session->prepareAudio();
        deviceManager.addAudioCallback(this);
    }

    void stopAudioStream() {
        deviceManager.removeAudioCallback(this);
        session->playState = Session::Play_State::Stopping;
        session->createFilesFromRecordedClips();
        assert(session->tracks.size() > 0);
        session->sessionXml->refreshXmlDocument(*session->playhead, session->tracks);
        session->playState = Session::Play_State::Stopped;
    }

    std::shared_ptr<Session> session;
    std::unique_ptr<AudioParams> params;

private:
    std::string sessionName;
    juce::AudioDeviceManager deviceManager;
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