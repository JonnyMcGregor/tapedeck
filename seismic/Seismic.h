#include "components/Session.h"
#include "components/XmlWrapper.h"

struct SeismicParams {
    SeismicParams(uint sample_rate, uint buffer_size, uint num_input_channels, uint num_output_channels) {
        this->sample_rate = sample_rate;
        this->buffer_size = buffer_size;
        this->num_input_channels = num_input_channels;
        this->num_output_channels = num_output_channels;
    }
    uint sample_rate, buffer_size, num_input_channels, num_output_channels;
};

//Seismic is the back-end interface which manages the DSP and project structure of the asciidaw
class Seismic {
public:
    Seismic(SeismicParams params, std::string project_name) {
        this->project_name = project_name;
        std::string xml_file_name = project_name + ".xml";
        session = std::make_unique<Session>(this->project_name, params.sample_rate, params.buffer_size, params.num_input_channels, params.num_output_channels);
        seismic_xml = std::make_unique<XmlWrapper>(this->project_name, xml_file_name, *session.get());
        createProjectFileStructure();
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

    void exportAllTracks() {
        for (int i = 0; i < session->record_armed_tracks.size(); i++) {
            assert(filesystem::exists(project_name + "/exported_audio"));
            for (auto &clip : session->record_armed_tracks[i]->clips) {
                std::ofstream audio_clip(project_name + "/exported_audio/" + clip.getName() + ".wav", std::ios::binary);
                session->wav_gen.openWaveFile(audio_clip);
                for (int sample = 0; sample < clip.getNumSamples(); sample++) {
                    session->wav_gen.writeInputToFile(audio_clip, clip.getSample(sample, session->wav_gen.getMaxAmplitude()));
                }
                session->wav_gen.closeWaveFile(audio_clip);
            }
        }
    }

    std::unique_ptr<Session> session;
    std::unique_ptr<XmlWrapper> seismic_xml;

private:
    std::string project_name;
    /* data */
};
