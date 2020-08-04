#include "Session.h"
#include "tiny_xml_2/tinyxml2.h"
#include <experimental/filesystem>
using namespace tinyxml2;

class XmlWrapper {
public:
    XmlWrapper(std::string project_name, std::string file_name, Session &session) {
        this->project_name = project_name;
        this->file_name = project_name + "/" + file_name;
        this->session = &session;
    }
    void createXMLDocument() {
        if (!std::experimental::filesystem::exists(project_name)) {
            std::experimental::filesystem::create_directory(project_name);
        }
        xml_doc.InsertFirstChild(session_node);
        xml_doc.SaveFile(file_name.c_str());
    }

    void refreshXMLDocument() {
        xml_doc.FirstChild()->DeleteChildren();
        addPlayheadElement();
        for (auto &track : session->tracks) {
            addTrackDataToXML(track);
        }
        xml_doc.SaveFile(file_name.c_str());
    }

    void addPlayheadElement() {
        playhead_element = xml_doc.NewElement("playhead");
        playhead_element->SetAttribute("tempo", to_string(120).c_str());
        playhead_element->SetAttribute("timeSigNumerator", to_string(4).c_str());
        playhead_element->SetAttribute("timeSigDenominator", to_string(4).c_str());
        playhead_element->SetAttribute("currentTimeSamples", to_string(session->getCurrentTimeInSamples()).c_str());
        playhead_element->SetAttribute("currentTimeSeconds", to_string(session->getCurrentTimeInSeconds()).c_str());
        session_node->InsertEndChild(playhead_element);
    }
    void addTrackDataToXML(Track &track) {
        track_element = xml_doc.NewElement("track");
        track_element->SetAttribute("name", track.getName().c_str());
        track_element->SetAttribute("isRecordEnabled", to_string(track.is_record_enabled).c_str());
        session_node->InsertEndChild(track_element);
        for (auto &clip : track.clips) {
            addClipToTrackElement(clip);
        }
    }

    void addClipToTrackElement(Clip &clip) {
        clip_element = xml_doc.NewElement("clip");
        addAttributesToClipElement(clip);
        track_element->InsertEndChild(clip_element);
    }

    void addAttributesToClipElement(Clip &clip) {
        clip_element->SetAttribute("name", clip.getName().c_str());
        clip_element->SetAttribute("reference_file_path", clip.getReferenceFilePath().c_str());
        clip_element->SetAttribute("start_time_in_session", clip.getStartTime());
        clip_element->SetAttribute("start_time_in_reference", clip.getStartTimeInReference());
        clip_element->SetAttribute("length_in_samples", clip.getNumSamples());
    }

private:
    XMLDocument xml_doc;
    XMLNode *session_node = xml_doc.NewElement("Session");
    XMLElement *track_element, *clip_element, *playhead_element;
    std::string project_name, file_name;
    Session *session;
};
