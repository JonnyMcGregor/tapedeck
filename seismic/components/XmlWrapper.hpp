#include "WaveFile.hpp"
#include "tiny_xml_2/tinyxml2.hpp"
#include "Playhead.hpp"
#include "Track.hpp"
#include <experimental/filesystem>
using namespace tinyxml2;

struct XmlWrapper {
    XmlWrapper(std::string session_name, std::string file_name) {
        this->session_name = session_name;
        this->file_name = session_name + "/" + file_name;
    }

    void create_xml_document() {
        if (!std::experimental::filesystem::exists(session_name)) {
            std::experimental::filesystem::create_directory(session_name);
        }
        session_node = xml_doc.NewElement("Session");
        xml_doc.InsertFirstChild(session_node);
        xml_doc.SaveFile(file_name.c_str());
    }

    void refresh_xml_document(Playhead &playhead, std::vector<std::shared_ptr<Track>> tracks) {
        xml_doc.FirstChild()->DeleteChildren();
        add_playhead_element(playhead, tracks.size());
        for (auto &track : tracks) {
            add_track_data_to_xml(*track);
        }
        xml_doc.SaveFile(file_name.c_str());
    }
    void set_elements() {
        session_node = xml_doc.RootElement();
        playhead_element = session_node->FirstChildElement();
        track_element = playhead_element->NextSiblingElement();
    }

    void add_playhead_element(Playhead playhead, int number_of_tracks) {
        playhead_element = xml_doc.NewElement("playhead");
        playhead_element->SetAttribute("tempo", std::to_string(playhead.tempo).c_str());
        playhead_element->SetAttribute("time_sig_numerator", std::to_string(playhead.time_sig_num).c_str());
        playhead_element->SetAttribute("time_sig_denominator", std::to_string(playhead.time_sig_denom).c_str());
        playhead_element->SetAttribute("current_time_samples", std::to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("current_time_seconds", std::to_string(playhead.current_time_in_samples).c_str());
        playhead_element->SetAttribute("number_of_tracks", number_of_tracks);
        session_node->InsertEndChild(playhead_element);
    }
    void add_track_data_to_xml(Track &track) {
        track_element = xml_doc.NewElement("track");
        track_element->SetAttribute("name", track.name.c_str());
        track_element->SetAttribute("is_record", std::to_string(track.record_armed).c_str());
        track_element->SetAttribute("is_solo", std::to_string(track.solo).c_str());
        track_element->SetAttribute("is_mute", std::to_string(track.mute).c_str());
        track_element->SetAttribute("number_of_clips", track.clips.size());
        session_node->InsertEndChild(track_element);
        for (int i = 0; i < track.clips.size(); i++) {
            add_clip_to_track_element(*track.clips[i], track.clip_metadata[i]);
        }
    }

    void add_clip_to_track_element(Clip &clip, ClipMetadata &clip_metadata) {
        clip_element = xml_doc.NewElement("clip");
        clip_element->SetAttribute("reference_file_path", (clip_metadata.reference_file_path).c_str());
        clip_element->SetAttribute("start_time_in_session", clip_metadata.start_time);
        clip_element->SetAttribute("start_time_in_reference", clip_metadata.start_time_in_reference);
        clip_element->SetAttribute("length_in_samples", clip.size());
        track_element->InsertEndChild(clip_element);
    }

    void load_playhead_data(Playhead &playhead) {
        playhead.movePlayhead(playhead_element->FindAttribute("current_time_samples")->IntValue());
        playhead.tempo = playhead_element->FindAttribute("tempo")->IntValue();
        playhead.time_sig_num = playhead_element->FindAttribute("time_sig_numerator")->IntValue();
        playhead.time_sig_denom = playhead_element->FindAttribute("time_sig_denominator")->IntValue();
    }

    std::vector<std::shared_ptr<Track>> load_tracks() {
        std::vector<std::shared_ptr<Track>> tracks;
        for (int i = 0; i < get_num_tracks(); i++) {
            tracks.push_back(std::shared_ptr<Track>());
            tracks.back() = std::make_shared<Track>();
            tracks.back()->name = ("Track" + std::to_string(tracks.size()));
            load_track_data(*tracks.back());
        }
        return tracks;
    }

    void load_track_data(Track &track) {
        for (int i = 0; i < track_element->FindAttribute("number_of_clips")->IntValue(); i++) {
            track.create_clip(clip_element->FindAttribute("start_time_in_session")->IntValue(), (session_name + "/recorded_audio/"));
            load_audio_clip(track.clips.back(), track.clip_metadata.back());
        }
        track_element = track_element->NextSiblingElement();
    }

    void load_audio_clip(std::shared_ptr<Clip> clip, ClipMetadata metadata) {
        WaveFile wave_file = {metadata.reference_file_path};
        clip->clear();
        for (int i = 0; i < clip->size(); i++) {
            int first_sample_index = 44;
            //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
            int sample_pos = first_sample_index + metadata.start_time_in_reference + (i * 2);
            //as samples are stored as little endian, both bytes within sample must be retrieved individually
            double max_amplitude = wave_file.read_byte_pair(34);
            double raw_sample = wave_file.read_byte_pair(sample_pos);
            //sample data is then converted to its original double between 1.0 and -1.0
            clip->append_sample(Sample(raw_sample / max_amplitude));
        }
    }

    int get_num_tracks() {
        return playhead_element->FindAttribute("number_of_tracks")->IntValue();
    }

    int get_selected_clip_length() {
        return clip_element->FindAttribute("length_in_samples")->IntValue();
    }

    XMLDocument xml_doc;
    XMLNode *session_node;
    XMLElement *track_element, *clip_element, *playhead_element;
    std::string session_name, file_name;
};
