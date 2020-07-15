#include "components/Session.h"
#include "tiny_xml_2/tinyxml2.h"
#include <experimental/filesystem>

struct SeismicParams {
    SeismicParams(uint sample_rate, uint buffer_size, uint num_input_channels, uint num_output_channels) {
        this->sample_rate = sample_rate;
        this->buffer_size = buffer_size;
        this->num_input_channels = num_input_channels;
        this->num_output_channels = num_output_channels;
    }
    uint sample_rate, buffer_size, num_input_channels, num_output_channels;
};

using namespace tinyxml2;
//Seismic is the back-end interface which manages the DSP and project structure of the asciidaw
class Seismic {
public:
    Seismic(SeismicParams params, std::string file_name) {
        session = std::make_unique<Session>(params.sample_rate, params.buffer_size, params.num_input_channels, params.num_output_channels);
        this->file_name = "Project/" + file_name + ".xml";
    }
    ~Seismic() {
    }

    void createXMLDocument() {
        if (!std::experimental::filesystem::exists("Project")) {
            std::experimental::filesystem::create_directory("Project");
        }
        project_xml.InsertFirstChild(session_node);
        project_xml.SaveFile(file_name.c_str());
    }

    void addTrackToXML(Track &track) {
        track_element = project_xml.NewElement("track");
        track_element->SetAttribute("name", track.getName().c_str());
        session_node->InsertEndChild(track_element);
        project_xml.SaveFile(file_name.c_str());
    }

    void addClipToXML(Clip &clip) {
        clip_element = project_xml.NewElement("clip");
        addAttributesToClipElement(clip);
        const XMLAttribute *clip_name = clip_element->FindAttribute("name");
        std::string track_name = clip_name->Value();
        addClipToTrackElement(track_name);
        project_xml.SaveFile(file_name.c_str());
    }

    void addAttributesToClipElement(Clip &clip) {
        clip_element->SetAttribute("name", clip.getName().c_str());
        clip_element->SetAttribute("reference_file_path", clip.getReferenceFilePath().c_str());
        clip_element->SetAttribute("start_time_in_session", clip.getStartTime());
        clip_element->SetAttribute("start_time_in_reference", clip.getStartTimeInReference());
        clip_element->SetAttribute("length_in_samples", clip.getNumSamples());
    }
    void addClipToTrackElement(std::string track_name) {
        track_element = session_node->LastChildElement();
        for (int i = session->tracks.size() - 1; i >= 0; i--) {
            int name_length = session->tracks[i].getName().length();
            track_name = track_name.substr(0, name_length);
            if (session->tracks[i].getName() == track_name) {
                if (track_element->FindAttribute("name")->Value() == track_name) {
                    track_element->InsertEndChild(clip_element);
                }
            } else {
                track_element = track_element->PreviousSiblingElement();
            }
        }
    }

    std::unique_ptr<Session> session;

private:
    std::string file_name;
    XMLDocument project_xml;
    XMLNode *session_node = project_xml.NewElement("Session");
    XMLElement *track_element, *clip_element;
    /* data */
};
