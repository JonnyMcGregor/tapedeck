#include "tiny_xml_2/tinyxml2.hpp"
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

    void refreshXMLDocument(Playhead &playhead, std::vector<Track> &tracks) {
        xml_doc.FirstChild()->DeleteChildren();
        addPlayheadElement(playhead, tracks.size());
        for (auto &track : tracks) {
            addTrackDataToXML(track);
        }
        xml_doc.SaveFile(file_name.c_str());
    }

    void addPlayheadElement(Playhead playhead, int number_of_tracks) {
        playhead_element = xml_doc.NewElement("playhead");
        playhead_element->SetAttribute("tempo", std::to_string(playhead.tempo).c_str());
        playhead_element->SetAttribute("time_sig_numerator", std::to_string(playhead.time_sig_num).c_str());
        playhead_element->SetAttribute("time_sig_denominator", std::to_string(playhead.time_sig_denom).c_str());
        playhead_element->SetAttribute("current_time_samples", std::to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("current_time_seconds", std::to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("number_of_tracks", number_of_tracks);
        session_node->InsertEndChild(playhead_element);
    }
    void addTrackDataToXML(Track &track) {
        track_element = xml_doc.NewElement("track");
        track_element->SetAttribute("name", track.name.c_str());
        track_element->SetAttribute("is_record_enabled", std::to_string(track.record_armed).c_str());
        track_element->SetAttribute("number_of_clips", track.clips.size());
        session_node->InsertEndChild(track_element);
        for (int i = 0; i < track.clips.size(); i++) {
            addClipToTrackElement(track.clips[i], track.start_times[i]);
        }
    }

    void addClipToTrackElement(Clip &clip, int start_time_in_session) {
        clip_element = xml_doc.NewElement("clip");
        addAttributesToClipElement(clip, start_time_in_session);
        track_element->InsertEndChild(clip_element);
    }

    void addAttributesToClipElement(Clip &clip, int start_time_in_session) {
        clip_element->SetAttribute("name", clip.name.c_str());
        clip_element->SetAttribute("reference_file_path", clip.reference_file_path.c_str());
        clip_element->SetAttribute("start_time_in_session", start_time_in_session);
        clip_element->SetAttribute("start_time_in_reference", clip.start_time_in_reference);
        clip_element->SetAttribute("length_in_samples", clip.size());
    }
    XMLDocument xml_doc;
    XMLNode *session_node;
    XMLElement *track_element, *clip_element, *playhead_element;
    std::string project_name, file_name;
};
