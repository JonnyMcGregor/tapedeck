#include "tiny_xml_2/tinyxml2.h"
#include <experimental/filesystem>

using namespace tinyxml2;

struct XmlWrapper {
    XmlWrapper(std::string session_name, std::string file_name) {
        this->project_name = session_name;
        this->file_name = session_name + "/" + file_name;
    }
    void createXMLDocument() {
        if (!std::experimental::filesystem::exists(project_name)) {
            std::experimental::filesystem::create_directory(project_name);
        }
        session_node = xml_doc.NewElement("Session");
        xml_doc.InsertFirstChild(session_node);
        xml_doc.SaveFile(file_name.c_str());
    }

    void refreshXMLDocument(Playhead &playhead, vector<Track> &tracks) {
        xml_doc.FirstChild()->DeleteChildren();
        addPlayheadElement(playhead, tracks.size());
        for (auto &track : tracks) {
            addTrackDataToXML(track);
        }
        xml_doc.SaveFile(file_name.c_str());
    }

    void addPlayheadElement(Playhead playhead, int number_of_tracks) {
        playhead_element = xml_doc.NewElement("playhead");
        playhead_element->SetAttribute("tempo", to_string(playhead.tempo).c_str());
        playhead_element->SetAttribute("time_sig_numerator", to_string(playhead.time_sig_num).c_str());
        playhead_element->SetAttribute("time_sig_denominator", to_string(playhead.time_sig_denom).c_str());
        playhead_element->SetAttribute("current_time_samples", to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("current_time_seconds", to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("number_of_tracks", number_of_tracks);
        session_node->InsertEndChild(playhead_element);
    }
    void addTrackDataToXML(Track &track) {
        track_element = xml_doc.NewElement("track");
        track_element->SetAttribute("name", track.getName().c_str());
        track_element->SetAttribute("is_record_enabled", to_string(track.is_record_enabled).c_str());
        track_element->SetAttribute("number_of_clips", track.clips.size());
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
    XMLDocument xml_doc;
    XMLNode *session_node;
    XMLElement *track_element, *clip_element, *playhead_element;
    std::string project_name, file_name;
};
