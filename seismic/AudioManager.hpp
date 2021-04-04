#include "components/Session.hpp"

#include <rtaudio/RtAudio.h>

//The back-end interface which manages the DSP and session structure of TapeDeck
class AudioManager {
public:
    AudioManager(bool is_loading_from_xml, std::string project_name = "", filesystem::path xml_path = "") {
        //Setup Audio Devices and Parameters
        check_for_available_devices();
        initialise_audio_io();
        params = std::make_unique<Audio_Params>(params->sample_rate, params->buffer_size, input_params.nChannels, output_params.nChannels);
        if (is_loading_from_xml) {
            this->session_name = xml_path.stem();
        } else {
            this->session_name = project_name;
        }
        session = std::make_shared<Session>(this->session_name, *params.get());
    }

    ~AudioManager() {
    }

    void check_for_available_devices() {
        if (dac.getDeviceCount() < 1) {
            std::cout << "No audio devices found, exiting ...\n";
            exit(0);
        }
    }

    void initialise_audio_io() {
        output_params.deviceId = dac.getDefaultOutputDevice();
        input_params.deviceId = dac.getDefaultInputDevice();

        output_info = dac.getDeviceInfo(output_params.deviceId);
        output_params.nChannels = output_info.outputChannels;
        output_params.firstChannel = 0;

        input_info = dac.getDeviceInfo(input_params.deviceId);
        input_params.nChannels = 1;
        input_params.firstChannel = 0;

        params->sample_rate = input_info.preferredSampleRate;
    }

    void start_audio_stream() {
        if (session->tracks.size() == 0)
            return;
        session->play_state = Session::Play_State::ToPlay;
        session->prepare_audio();
        dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                       params->sample_rate, &params->buffer_size, &process_audio_block, session.get());
        dac.startStream();
    }

    void stop_audio_stream() {
        session->play_state = Session::Play_State::Stopping;
        dac.stopStream();
        session->create_files_from_recorded_clips();
        assert(session->tracks.size() > 0);
        session->session_xml->refresh_xml_document(session->playhead, session->tracks);
        session->play_state = Session::Play_State::Stopped;
        dac.closeStream();
    }

    static int process_audio_block(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                                   double streamTime, RtAudioStreamStatus status, void *userData) {
        unsigned int sample_index, channel_index;
        double *out_buffer = (double *)outputBuffer;
        double *in_buffer = (double *)inputBuffer;
        Session *session = (Session *)userData;

        if (status)
            std::cout << "Stream underflow detected!" << std::endl;
        //Temporary for testing purposes...
        //If tracks are armed, record audio
        if (session->num_record_armed_tracks() > 0)
            session->play_state = Session::Play_State::Recording;
        //Else playback audio
        else
            session->play_state = Session::Play_State::Playing;

        session->process_audio_block(in_buffer, out_buffer);
        return 0;
    }

    std::shared_ptr<Session> session;

private:
    std::string session_name;
    std::unique_ptr<Audio_Params> params;
    RtAudio dac;
    RtAudio::StreamParameters output_params, input_params;
    RtAudio::DeviceInfo output_info, input_info;
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