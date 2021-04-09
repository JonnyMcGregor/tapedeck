#include "components/Session.hpp"
#include "juce_modules/juce_audio_devices/juce_audio_devices.h"
#include <rtaudio/RtAudio.h>

//The back-end interface which manages the DSP and session structure of TapeDeck
class AudioManager : juce::AudioIODeviceCallback {
public:
    AudioManager(bool is_loading_from_xml, std::string project_name = "", filesystem::path xml_path = "") {
        //Setup Audio Devices and Parameters
        device_manager.initialiseWithDefaultDevices(1, 2);
        juce::AudioIODevice* current_device = device_manager.getCurrentAudioDevice();
        params = std::make_unique<AudioParams>(current_device->getCurrentSampleRate(), 
                    current_device->getCurrentBufferSizeSamples(), 1, 2);
        if (is_loading_from_xml) {
            this->session_name = xml_path.stem();
        } else {
            this->session_name = project_name;
        }
        session = std::make_shared<Session>(this->session_name, *params.get());
    }

    ~AudioManager() {
    }

    void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples) {

        Buffer out_buffer = {outputChannelData};
        Buffer in_buffer = {inputChannelData};
        
        //If tracks are armed, record audio
        if (session->num_record_armed_tracks() > 0)
            session->play_state = Session::Play_State::Recording;
        //Else playback audio
        else
            session->play_state = Session::Play_State::Playing;

        session->process_audio_block(in_buffer, out_buffer);
    }

    void audioDeviceAboutToStart(juce::AudioIODevice *device) {
    }

    /** Called to indicate that the device has stopped. */
    void audioDeviceStopped(){
    }

    void start_audio_stream() {
        if (session->tracks.size() == 0)
            return;
        session->play_state = Session::Play_State::ToPlay;
        session->prepare_audio();
        device_manager.addAudioCallback(this);

        // dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
        //                params->sample_rate, &params->buffer_size, &process_audio_block, session.get());
        // dac.startStream();
    }

    void stop_audio_stream() {
        device_manager.removeAudioCallback(this);
        session->play_state = Session::Play_State::Stopping;
        session->create_files_from_recorded_clips();
        assert(session->tracks.size() > 0);
        session->session_xml->refresh_xml_document(*session->playhead, session->tracks);
        session->play_state = Session::Play_State::Stopped;
    }

    std::shared_ptr<Session> session;
    std::unique_ptr<AudioParams> params;

private:
    std::string session_name;
    juce::AudioDeviceManager device_manager;
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