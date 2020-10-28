#include "components/Session.h"

#include <rtaudio/RtAudio.h>

//Seismic is the back-end interface which manages the DSP and project structure of the asciidaw
class Seismic {
public:
    Seismic(bool is_loading_from_xml, std::string project_name = "", filesystem::path xml_path = "") {
        //Setup Audio Devices and Parameters
        if (dac.getDeviceCount() < 1) {
            std::cout << "No audio devices found, exiting ...\n";
            exit(0);
        }
        initialiseAudioIO();
        params = std::make_unique<SeismicParams>(sample_rate, buffer_size, input_params.nChannels, output_params.nChannels);
        if (is_loading_from_xml) {
            this->project_name = xml_path.stem();
        } else {
            this->project_name = project_name;
        }
        createProjectFileStructure();
        session = std::make_shared<Session>(this->project_name, *params.get());
    }

    ~Seismic() {
    }

    void createProjectFileStructure() {
        if (!filesystem::exists(project_name))
            filesystem::create_directory(project_name);
        if (!filesystem::exists(project_name + "/recorded_audio"))
            filesystem::create_directory(project_name + "/recorded_audio");
        if (!filesystem::exists(project_name + "/exported_audio"))
            filesystem::create_directory(project_name + "/exported_audio");
    }

    void initialiseAudioIO() {
        output_params.deviceId = dac.getDefaultOutputDevice();
        input_params.deviceId = dac.getDefaultInputDevice();

        output_info = dac.getDeviceInfo(output_params.deviceId);
        output_params.nChannels = output_info.outputChannels;
        output_params.firstChannel = 0;

        input_info = dac.getDeviceInfo(input_params.deviceId);
        input_params.nChannels = 1;
        input_params.firstChannel = 0;

        sample_rate = input_info.preferredSampleRate;
    }

    void startAudioStream() {
        if (session->tracks.size() == 0)
            return;
        session->play_state = Session::Play_State::ToPlay;
        session->prepareAudio();
        dac.openStream(&output_params, &input_params, RTAUDIO_FLOAT64,
                       sample_rate, &buffer_size, &processAudioBlock, session.get());
        dac.startStream();
    }

    void stopAudioStream() {
        session->play_state = Session::Play_State::Stopping;
        dac.stopStream();
        session->createFilesFromRecordedClips();
        assert(session->tracks.size() > 0);
        session->session_xml->refreshXMLDocument(session->playhead, session->tracks);
        session->play_state = Session::Play_State::Stopped;
        dac.closeStream();
    }

    static int processAudioBlock(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                                 double streamTime, RtAudioStreamStatus status, void *userData) {
        unsigned int sample_index, channel_index;
        double *out_buffer = (double *)outputBuffer;
        double *in_buffer = (double *)inputBuffer;
        Session *session = (Session *)userData;

        if (status)
            std::cout << "Stream underflow detected!" << std::endl;
        //Temporary for testing purposes...
        //If tracks are armed, record audio
        if (session->record_armed_tracks.size() > 0)
            session->play_state = Session::Play_State::Recording;
        //Else playback audio
        else
            session->play_state = Session::Play_State::Playing;

        session->processAudioBlock(in_buffer, out_buffer);
        return 0;
    }

    void exportAllTracks() {
        for (int i = 0; i < session->record_armed_tracks.size(); i++) {
            assert(filesystem::exists(project_name + "/exported_audio"));
            for (auto &clip : session->record_armed_tracks[i]->clips) {
                std::ofstream audio_clip(project_name + "/exported_audio/" + clip.getName() + ".wav", std::ios::binary);
                session->wav_gen.openWaveFile(audio_clip);
                for (int sample = 0; sample < clip.getNumSamples(); sample++) {
                    session->wav_gen.writeInputToFile(audio_clip, clip.getSample(sample));
                }
                session->wav_gen.closeWaveFile(audio_clip);
            }
        }
    }

    std::shared_ptr<Session> session;
    std::unique_ptr<SeismicParams> params;

private:
    std::string project_name;

    RtAudio dac;
    RtAudio::StreamParameters output_params, input_params;
    RtAudio::DeviceInfo output_info, input_info;
    unsigned int sample_rate = 44100, buffer_size = 256;
};
